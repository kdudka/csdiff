/*
 * Copyright (C) 2011 - 2021 Red Hat, Inc.
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

#ifndef H_GUARD_INSTREAM_H
#define H_GUARD_INSTREAM_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct InFileException {
    std::string fileName;
    // TODO: details (errno?)

    InFileException(const std::string &fileName_):
        fileName(fileName_)
    {
    }
};

class InStream {
    public:
        InStream(const std::string &fileName, bool silent = false);
        InStream(std::istringstream &str, bool silent = false);
        ~InStream();

        const std::string& fileName()   const { return fileName_;   }
        std::istream& str()             const { return str_;        }
        bool silent()                   const { return silent_;     }
        bool anyError()                 const { return anyError_;   }

        void handleError(std::string msg = std::string(), long line = 0L);

    private:
        const std::string   fileName_;
        const bool          silent_;
        bool                anyError_;
        std::fstream        fileStr_;
        std::istream       &str_;
};

class InStreamLookAhead {
    public:
        InStreamLookAhead(
                InStream       &inStr,
                unsigned        size,
                bool            skipWhiteSpaces = false);

        char operator[](const unsigned idx) const {
            return buf_.at(idx);
        }

    private:
        std::vector<char> buf_;
};

#endif /* H_GUARD_INSTREAM_H */
