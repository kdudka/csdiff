/*
 * Copyright (C) 2011 Red Hat, Inc.
 *
 * This file is part of csdiff.
 *
 * csdiff is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * csdiff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with csdiff.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "abstract-filter.hh"

#include "instream.hh"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

#include <boost/foreach.hpp>

// /////////////////////////////////////////////////////////////////////////////
// implementation of PredicateFilter

struct PredicateFilter::Private {
    typedef std::vector<IPredicate *>               TList;
    bool                invertEach_;
    TList               preds_;

    Private():
        invertEach_(false)
    {
    }
};

PredicateFilter::PredicateFilter(AbstractWriter *slave):
    AbstractFilter(slave),
    d(new Private)
{
}

PredicateFilter::~PredicateFilter() {
    BOOST_FOREACH(IPredicate *pred, d->preds_)
        delete pred;

    delete d;
}

void PredicateFilter::append(IPredicate *pred) {
    d->preds_.push_back(pred);
}

void PredicateFilter::setInvertEachMatch(bool enabled) {
    d->invertEach_ = enabled;
}

bool PredicateFilter::matchDef(const Defect &def) {
    const bool neg = d->invertEach_;

    BOOST_FOREACH(const IPredicate *pred, d->preds_) {
        if (neg == pred->matchDef(def))
            return false;
    }

    return true;
}


// /////////////////////////////////////////////////////////////////////////////
// implementation of EventPrunner

void EventPrunner::handleDef(const Defect &defOrig) {
    Defect def(defOrig);
    def.events.clear();

    const unsigned cnt = defOrig.events.size();
    for (unsigned i = 0; i < cnt; ++i) {
        const DefEvent &evt = defOrig.events[i];

        if (evt.verbosityLevel <= thr_)
            def.events.push_back(evt);
        else if (i < defOrig.keyEventIdx)
            def.keyEventIdx--;
    }

    slave_->handleDef(def);
}


// /////////////////////////////////////////////////////////////////////////////
// implementation of CtxEmbedder

void dropCtxLines(TEvtList *pEvtList) {
    static CtxEventDetector detector;

    TEvtList dst;
    BOOST_FOREACH(const DefEvent &evt, *pEvtList) {
        if (detector.isAnyCtxLine(evt))
            continue;

        dst.push_back(evt);
    }

    pEvtList->swap(dst);
}

void appendCtxLines(
        TEvtList       *pDst,
        std::istream   &inStr,
        const int       defLine,
        const int       ctxLines)
{
    if (ctxLines < 0)
        return;

    const int firstLine = defLine - ctxLines;
    const int lastLine  = defLine + ctxLines;

    int line = 1;
    std::string text;
    for (; line <= lastLine; ++line) {
        if (!std::getline(inStr, text))
            // premature end of input
            break;

        if (line < firstLine)
            // skip lines before the context lines
            continue;

        // format a single line of the comment
        std::ostringstream str;
        str << std::fixed << std::setw(5) << line;
        if (defLine == line)
            str << "|-> ";
        else
            str << "|   ";
        str << text;

        // append the comment as a new event
        DefEvent evt;
        evt.event = "#";
        evt.msg = str.str();
        evt.verbosityLevel = /* not a key event */ 1;
        pDst->push_back(evt);
    }
}

void CtxEmbedder::handleDef(const Defect &defOrig) {
    const DefEvent &evt = defOrig.events[defOrig.keyEventIdx];
    if (!evt.line) {
        // no line number for the key event
        slave_->handleDef(defOrig);
        return;
    }

    std::fstream fstr(evt.fileName.c_str(), std::ios::in);
    if (!fstr) {
        // failed to open input file
        slave_->handleDef(defOrig);
        return;
    }

    // clone defOrig and append the context lines
    Defect def(defOrig);
    dropCtxLines(&def.events);
    appendCtxLines(&def.events, fstr, evt.line, ctxLines_ - 1);

    // close the file stream and forward the result
    fstr.close();
    slave_->handleDef(def);
}
