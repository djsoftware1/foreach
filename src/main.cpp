// https://github.com/djsoftware1/foreach
/*
 * foreach — a small, cross-platform Unix-style utility
 *
 * Iterate over lines from stdin and execute a command for each line,
 * with simple positional variable expansion ($1, $2, $*, $#).
 *
 * Default behavior treats each input line as a single value unless
 * tab-separated fields are present. Whitespace splitting is opt-in.
 *
 * This tool is intentionally small and limited in scope. It is not a
 * scripting language, task runner, or shell replacement.
 *
 * Author: David Joffe
 * URL:    https://github.com/djsoftware1/foreach
 *
 * Copyright (c) 2026 David Joffe
 * License: MIT License (see LICENSE file)
 *
 * Created: 2 Jan 2026
 */

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <cstdint>
#include <cstdlib>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
  #include <sys/wait.h>
#endif

#include "help.h"

// ---------- utilities ----------

static std::string trim(std::string_view s) {
    size_t start = 0;
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t'))
        ++start;

    size_t end = s.size();
    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t'))
        --end;

    return std::string(s.substr(start, end - start));
}

static std::vector<std::string> split_whitespace(std::string_view line) {
    std::vector<std::string> out;
    size_t i = 0;

    while (i < line.size()) {
        while (i < line.size() && (line[i] == ' ' || line[i] == '\t'))
            ++i;
        if (i >= line.size())
            break;
        size_t j = i;
        while (j < line.size() && line[j] != ' ' && line[j] != '\t')
            ++j;
        out.emplace_back(line.substr(i, j - i));
        i = j;
    }
    return out;
}

static std::vector<std::string> split_delim(std::string_view line, char delim) {
    std::vector<std::string> out;
    size_t pos = 0;

    while (true) {
        size_t next = line.find(delim, pos);
        if (next == std::string_view::npos) {
            out.emplace_back(trim(line.substr(pos)));
            break;
        }
        out.emplace_back(trim(line.substr(pos, next - pos)));
        pos = next + 1;
    }
    return out;
}

static bool has_placeholder(const std::vector<std::string>& args) {
    for (const auto& s : args) {
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '$' && i + 1 < s.size()) {
                char c = s[i + 1];
                if (c == '*' || c == '#' || std::isdigit((unsigned char)c)) {
                    return true;
                }
            }
        }
    }
    return false;
}

static std::string expand_template(
    std::string_view tmpl,
    const std::vector<std::string>& fields,
    const std::string& raw_line,
    uint64_t line_no
) {
    std::string out;
    for (size_t i = 0; i < tmpl.size(); ++i) {
        if (tmpl[i] == '$' && i + 1 < tmpl.size()) {
            if (tmpl[i + 1] == '*') {
                out += raw_line;
                ++i;
            } else if (tmpl[i + 1] == '#') {
                out += std::to_string(line_no);
                ++i;
            } else if (std::isdigit((unsigned char)tmpl[i + 1])) {
                size_t j = i + 1;
                size_t n = 0;
                while (j < tmpl.size() && std::isdigit((unsigned char)tmpl[j])) {
                    n = n * 10 + (tmpl[j] - '0');
                    ++j;
                }
                if (n >= 1 && n <= fields.size())
                    out += fields[n - 1];
                i = j - 1;
            } else {
                out += tmpl[i];
            }
        } else {
            out += tmpl[i];
        }
    }
    return out;
}

#ifdef _WIN32
static std::wstring utf8_to_utf16(const std::string& s) {
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::wstring out(len - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, out.data(), len);
    return out;
}

// Correct Windows quoting per MS rules
static std::wstring quote_arg_windows(const std::wstring& arg) {
    std::wstring out = L"\"";
    size_t bs = 0;

    for (wchar_t c : arg) {
        if (c == L'\\') {
            ++bs;
        } else {
            if (c == L'"')
                out.append(bs * 2 + 1, L'\\');
            else if (bs)
                out.append(bs, L'\\');
            out.push_back(c);
            bs = 0;
        }
    }
    if (bs)
        out.append(bs * 2, L'\\');
    out.push_back(L'"');
    return out;
}
#endif

// ---------- main ----------

int main(int argc, char* argv[]) {
    bool space_delim = false;
    std::optional<char> delim;

    std::vector<std::string> cmd_template;

    // PARSE COMMAND LINE ARGUMENTS
    // "--" indicates end of argument parsing in case anyone ever wants to pass things like "-h" as actual arguments ... not impossible. -- allows that. dj2026-01-05
    bool end_of_opts = false;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
    
        if (!end_of_opts && a == "--") {
            end_of_opts = true;
            continue;
        }
    
        if (!end_of_opts) {
            if (a == "-h" || a == "--help") {
                print_help();
                return 0;
            }
            if (a == "--version") {
                print_version();
                return 0;
            }
            // Space delimiter
            if (a == "-s" || a == "--space-delim") {
                space_delim = true;
                continue;
            }
            // Custom delimiter
            if (a == "--delim" && i + 1 < argc) {
                delim = argv[++i][0];
                continue;
            }
            /*
            if (a == "--include-empty") {
                include_empty = true;
                continue;
            }
            */
            // other flags...
        }
    
        // Everything else is part of the command template
        cmd_template.push_back(a);

    }    

    if (cmd_template.empty()) {
        std::cerr << "for-each: no command given\n";
        return 1;
    }

    std::string line;
    bool first_line = true;
    uint64_t line_no = 0;

    while (std::getline(std::cin, line)) {
        ++line_no;

        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        // UTF-8 BOM stripping (once)
        if (first_line) {
            first_line = false;
            if (line.size() >= 3 &&
                (unsigned char)line[0] == 0xEF &&
                (unsigned char)line[1] == 0xBB &&
                (unsigned char)line[2] == 0xBF) {
                line.erase(0, 3);
            }
        }

        std::string raw_line = line;

        // Trim ONLY to decide emptiness
        std::string trimmed = trim(line);
        if (trimmed.empty())
            continue;
        
        // Use trimmed for splitting logic
        std::string processed = trimmed;
        
        std::vector<std::string> fields;
    
        if (space_delim) {
            fields = split_whitespace(processed);
        } else if (delim) {
            fields = split_delim(processed, *delim);
        } else {
            // default: TAB-based
            if (processed.find('\t') != std::string::npos)
                fields = split_delim(processed, '\t');
            else
                fields = { processed };
        }

        // shouldn't happen but just in case ..
        if (fields.empty())
            continue;

        std::vector<std::string> cmd;
        for (const auto& a : cmd_template)
        {
            cmd.push_back(expand_template(a, fields, raw_line, line_no));
        }

        // 'implicit mode' ... if user specifies expansions then don't append the full params by default or we get an extra one
        // for example 'git clone $1 b_$1' becomes in advertently:
        // 'git clone $1 b_$1' $*
        // as in the normal case like 'for-each git clone' it correctly appends $*
        // but for cases like git clone $1 b_$1' that auto-appending will mess us around as the extra parameter(s) to git would cause fail
        bool uses_placeholders = has_placeholder(cmd_template);
        if (!uses_placeholders) {
            cmd.push_back(raw_line);
        }
            
#ifdef _WIN32
        std::wstring cmdline;
        for (size_t i = 0; i < cmd.size(); ++i) {
            if (i) cmdline += L' ';
            cmdline += quote_arg_windows(utf8_to_utf16(cmd[i]));
        }

        STARTUPINFOW si{};
        PROCESS_INFORMATION pi{};
        si.cb = sizeof(si);

        if (!CreateProcessW(
                nullptr, cmdline.data(), nullptr, nullptr,
                FALSE, 0, nullptr, nullptr, &si, &pi)) {
            std::cerr << "foreach: CreateProcess failed\n";
            return 1;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
#else
        std::vector<char*> argv_exec;
        for (auto& s : cmd)
            argv_exec.push_back(const_cast<char*>(s.c_str()));
        argv_exec.push_back(nullptr);

        pid_t pid = fork();
        if (pid == 0) {
            execvp(argv_exec[0], argv_exec.data());
            _exit(127);
        } else if (pid < 0) {
            perror("fork");
            return 1;
        }

        int status;
        waitpid(pid, &status, 0);
#endif
    }

    return 0;
}
