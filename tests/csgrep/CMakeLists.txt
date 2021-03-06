# Copyright (C) 2011 - 2020 Red Hat, Inc.
#
# This file is part of csdiff.
#
# csdiff is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# csdiff is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with csdiff.  If not, see <http://www.gnu.org/licenses/>.

# a generic template for csparser test-cases
macro(test_csparser dir num)
    set(tst "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${num}")

    set(cmd "${csgrep} ${tst}-stdin.txt 2>/dev/null")
    set(cmd "${cmd} | ${diffcmd} ${tst}-stdout.txt -")
    add_test_wrap("csgrep/${dir}-${num}-stdout" "${cmd}")

    set(cmd "${csgrep} ${tst}-stdin.txt 2>&1 >/dev/null")
    set(cmd "${cmd} | sed -s 's|^[^:]*/||' | ${diffcmd} ${tst}-stderr.txt -")
    add_test_wrap("csgrep/${dir}-${num}-stderr" "${cmd}")
endmacro()

# a generic template for csgrep test-cases
macro(test_csgrep num)
    set(tst "${CMAKE_CURRENT_SOURCE_DIR}/${num}")

    file(READ ${tst}-args.txt args)
    string(REPLACE "\n" "" args "${args}")
    set(cmd "${csgrep} ${args} ${tst}-stdin.txt")
    set(cmd "${cmd} | ${jsfilter} | ${diffcmd} ${tst}-stdout.txt -")
    add_test_wrap("csgrep/${num}" "${cmd}")
endmacro()

# csgrep tests
test_csparser(csparser-5.8                          00)
test_csparser(csparser-5.8                          01)
test_csparser(csparser-5.8                          02)
test_csgrep("00-exclude-conftest"                     )
test_csgrep("01-remove-duplicates"                    )
test_csgrep("02-compiler-warnings"                    )
test_csgrep("03-compiler-warnings"                    )
test_csgrep("04-compiler-warnings"                    )
test_csgrep("05-compiler-warnings"                    )
test_csgrep("06-cppcheck-warnings"                    )
test_csgrep("07-cppcheck-warnings"                    )
test_csgrep("08-cppcheck-warnings"                    )
test_csgrep("09-err-file-comments"                    )
test_csgrep("10-err-file-comments"                    )
test_csgrep("11-err-file-comments"                    )
test_csgrep("12-llvm-build-warnings"                  )
test_csgrep("13-llvm-build-warnings"                  )
test_csgrep("14-llvm-build-warnings"                  )
test_csgrep("15-llvm-build-warnings"                  )
test_csgrep("16-compiler-warnings"                    )
test_csgrep("17-compiler-warnings"                    )
test_csgrep("18-compiler-warnings"                    )
test_csgrep("19-clang-warnings"                       )
test_csgrep("20-clang-warnings"                       )
test_csgrep("21-compiler-warnings"                    )
test_csgrep("22-compiler-warnings"                    )
test_csgrep("23-clang-warnings"                       )
test_csgrep("24-compiler-warnings"                    )
test_csgrep("25-compiler-warnings"                    )
test_csgrep("26-cov-format-errors"                    )
test_csgrep("27-cov-format-errors"                    )
test_csgrep("28-cov-format-errors"                    )
test_csgrep("29-cov-format-errors"                    )
test_csgrep("30-prospector-raw"                       )
test_csgrep("31-prospector-err"                       )
test_csgrep("32-shellcheck-raw"                       )
test_csgrep("33-cov-format-errors"                    )
test_csgrep("34-csgrep-color"                         )
test_csgrep("35-cov-format-errors"                    )
test_csgrep("36-csgrep-json"                          )
test_csgrep("37-csgrep-json"                          )
test_csgrep("38-csparser-remediation"                 )
test_csgrep("39-csparser-remediation"                 )
test_csgrep("40-csparser-code-snippet"                )
test_csgrep("41-gcc-parser-pylint"                    )
test_csgrep("42-gcc-parser-smatch"                    )
test_csgrep("43-gcc-parser-smatch"                    )
test_csgrep("44-csparser-new-key-evts"                )
test_csgrep("45-gcc-parser-clang"                     )
test_csgrep("46-cov-json-v2"                          )
test_csgrep("47-csparser-new-key-evts"                )
test_csgrep("48-csparser-missing-break-key-evt"       )
test_csgrep("49-csparser-findbugs-jsr166"             )
test_csgrep("50-gcc-parser-gcc-9.2.1"                 )
test_csgrep("51-gcc-parser-systemd"                   )
test_csgrep("52-gcc-parser-clang-nss"                 )
test_csgrep("53-csparser-note-events"                 )
test_csgrep("54-csparser-unknown-lineno"              )
test_csgrep("55-csparser-pylint-copr"                 )
test_csgrep("56-gcc-sparser-clang-error"              )
test_csgrep("57-gcc-parser-gcc-analyzer-curl"         )
test_csgrep("58-csparser-gcc-analyzer-curl"           )
test_csgrep("59-json-parser-cov-v7-cwe"               )
test_csgrep("60-gcc-parser-cppcheck-cwe"              )
test_csgrep("61-json-parser-cov-v7-lang"              )
test_csgrep("62-csparser-checker-lang"                )
test_csgrep("63-gcc-parser-checker-lang"              )
test_csgrep("64-json-parser-unknown-node"             )
test_csgrep("65-gcc-parser-clang-warn-suff"           )
test_csgrep("66-json-parser-valgrind-dedup"           )
test_csgrep("67-json-parser-valgrind-dedup"           )
test_csgrep("68-gcc-parser-cppcheck-iterator1"        )
test_csgrep("69-csfilter-python-build-path"           )
test_csgrep("70-csparser-included-from-note"          )
test_csgrep("71-csgrep-drop-scan-props"               )
test_csgrep("72-abstract-parser-gcc-builtin"          )
test_csgrep("73-coverity-misra-checkers"              )
test_csgrep("74-coverity-leading-space"               )
test_csgrep("75-coverity-from-cshtml"                 )
