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

#include "csparser.hh"
#include "deflookup.hh"
#include "instream.hh"

#include <cstdlib>

#include <boost/program_options.hpp>

int main(int argc, char *argv[])
{
    using std::string;
    const char *name = argv[0];

    namespace po = boost::program_options;
    po::variables_map vm;
    po::options_description desc(string("Usage: ") + name
            + " [options] old.err new.err, where options are");

    typedef std::vector<string> TStringList;
    string mode;

    try {
        desc.add_options()
            ("fixed,x", "print fixed defects (just swaps the arguments)")
            ("quiet,q", "do not report any parsing errors")
            ("help", "produce help message");

        po::options_description hidden("");
        hidden.add_options()
            ("input-file", po::value<TStringList>(), "input file");
        po::positional_options_description p;
        p.add("input-file", -1);

        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    

        po::options_description opts;
        opts.add(desc).add(hidden);
        po::store(po::command_line_parser(argc, argv).
                options(opts).positional(p).run(), vm);
        po::notify(vm);
    }
    catch (po::error &e) {
        std::cerr << name << ": error: " << e.what() << "\n\n";
        desc.print(std::cerr);
        return 1;
    }

    if (vm.count("help")) {
        desc.print(std::cout);
        return 0;
    }

    if (!vm.count("input-file")) {
        desc.print(std::cerr);
        return 1;
    }

    const TStringList &files = vm["input-file"].as<TStringList>();
    if (2 != files.size()) {
        desc.print(std::cerr);
        return 1;
    }

    const bool swap = vm.count("fixed");
    const string &fnOld = files[swap];
    const string &fnNew = files[!swap];

    const bool silent = vm.count("quiet");

    try {
        // open streams
        InStream strOld(fnOld.c_str());
        InStream strNew(fnNew.c_str());

        // read old
        Parser pOld(strOld.str(), fnOld, silent);
        DefLookup stor;
        Defect def;
        while (pOld.getNext(&def))
            stor.hashDefect(def);

        // read new
        Parser pNew(strNew.str(), fnNew, silent);
        while (pNew.getNext(&def)) {
            if (stor.lookup(def))
                continue;

            // a newly added defect found
            std::cout << def;
        }

        return pOld.hasError()
            || pNew.hasError();
    }
    catch (const InFileException &e) {
        std::cerr << e.fileName << ": failed to open input file\n";
        return EXIT_FAILURE;
    }
}
