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

#include "abstract-writer.hh"

#include "cswriter.hh"
#include "instream.hh"
#include "json-writer.hh"

// /////////////////////////////////////////////////////////////////////////////
// implementation of AbstractWriter

bool AbstractWriter::handleFile(const std::string &fileName, bool silent) {
    try {
        InStream str(fileName.c_str());

        this->notifyFile(fileName);

        // detect the input format and create the parser
        Parser parser(str.str(), fileName, silent);
        if (inputFormat_ == FF_INVALID)
            inputFormat_ = parser.inputFormat();

        Defect def;
        while (parser.getNext(&def))
            this->handleDef(def);

        return !parser.hasError();
    }
    catch (const InFileException &e) {
        std::cerr << e.fileName << ": failed to open input file\n";
        return false;
    }
}

AbstractWriter* createWriter(const EFileFormat format) {
    switch (format) {
        case FF_INVALID:
        case FF_COVERITY:
            return new CovWriter;

        case FF_JSON:
            return new JsonWriter;
    }

    return 0;
}