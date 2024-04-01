/*
 * Copyright (C) 2011-2023 Red Hat, Inc.
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

#include "deflookup.hh"

#include "msg-filter.hh"
#include "parser.hh"

#include <map>

typedef std::vector<Defect>                     TDefList;
typedef std::map<std::string, TDefList>         TDefByMsg;
typedef std::map<std::string, TDefByMsg>        TDefByEvt;
typedef std::map<std::string, TDefByEvt>        TDefByFile;
typedef std::map<std::string, TDefByFile>       TDefByChecker;

struct DefLookup::Private {
    TDefByChecker                    byChecker;
    bool                             usePartialResults;
};

DefLookup::DefLookup(const bool usePartialResults):
    d(new Private)
{
    d->usePartialResults = usePartialResults;
}

DefLookup::DefLookup(const DefLookup &ref):
    d(new Private(*ref.d))
{
}

DefLookup& DefLookup::operator=(const DefLookup &ref)
{
    if (&ref == this)
        return *this;

    delete d;
    d = new Private(*ref.d);
    return *this;
}

DefLookup::~DefLookup()
{
    delete d;
}

void DefLookup::hashDefect(const Defect &def)
{
    // categorize by checker
    TDefByFile &byPath = d->byChecker[def.checker];

    // categorize by path
    const DefEvent &evt = def.events[def.keyEventIdx];
    const MsgFilter &filter = MsgFilter::inst();
    TDefByEvt &byEvt = byPath[filter.filterPath(evt.fileName)];

    // categorize by key event and msg
    TDefByMsg &byMsg = byEvt[evt.event];
    TDefList &defList = byMsg[filter.filterMsg(evt.msg, def.checker)];

    defList.push_back(def);
}

static bool defLookupCore(TDefList &defList)
{
    // just remove an arbitrary one
    // TODO: add some other criteria in order to make the match more precise
    unsigned cnt = defList.size();
    if (cnt)
        defList.resize(cnt - 1);
    else
        return false;

    return true;
}

bool DefLookup::lookup(const Defect &def)
{
    // look for defect class
    TDefByChecker::iterator itByChecker = d->byChecker.find(def.checker);
    if (d->byChecker.end() == itByChecker)
        return false;

    // simplify path
    const MsgFilter &filter = MsgFilter::inst();
    const DefEvent &evt = def.events[def.keyEventIdx];
    const std::string path = filter.filterPath(evt.fileName);

    // look for file name
    TDefByFile &byPath = itByChecker->second;
    TDefByFile::iterator itByPath = byPath.find(path);
    if (byPath.end() == itByPath)
        return false;

    TDefByEvt &byEvt = itByPath->second;
    if (!d->usePartialResults && byEvt.end() != byEvt.find("internal warning"))
        // if the analyzer produced an "internal warning" diagnostic message,
        // we assume partial results, which cannot be reliably used for
        // differential scan ==> pretend we found what we had been looking
        // for, but do not remove anything from the store
        return true;

    // look by key event
    TDefByEvt::iterator itByEvent = byEvt.find(evt.event);
    if (byEvt.end() == itByEvent)
        return false;

    // look by msg
    TDefByMsg &byMsg = itByEvent->second;
    const std::string msg = filter.filterMsg(evt.msg, def.checker);
    TDefByMsg::iterator itByMsg = byMsg.find(msg);
    if (byMsg.end() == itByMsg)
        return false;

    // process the resulting list of defects sequentially
    TDefList &defList = itByMsg->second;
    if (!defLookupCore(defList))
        return false;

    // found!
    return true;
}
