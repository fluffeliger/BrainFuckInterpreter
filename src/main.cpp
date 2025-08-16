/**
 * @file main.cpp
 *
 * Made with <3 by fluffy
 */

#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <conio.h>
#include <cstring>

#define BRAINFUCK_THREAD_ARRAY_SIZE 65535

class BrainFuckThread {
    public:
        BrainFuckThread(const std::string &source, bool use_special)
        : _source(source), _source_size(source.size()), _use_special(use_special) {}

        ~BrainFuckThread() {
            delete[] _memory;
        }

        bool lex() {
            std::vector<size_t> loop_stack;

            for (size_t i = 0; i < _source_size; i++) {
                char ch = _source[i];

                if (ch == '[') {
                    loop_stack.push_back(i);
                    continue;
                }

                if (ch == ']') {
                    if (loop_stack.empty()) {
                        std::cerr << "Unmatched ']' at position " << i << std::endl;
                        return false;
                    }

                    size_t loop_start = loop_stack.back();
                    loop_stack.pop_back();
                    _loop_map[i] = loop_start;
                    _loop_map[loop_start] = i;
                }
            }

            if (!loop_stack.empty()) {
                std::cerr << "Unmatched '[' at position " << loop_stack.back() << std::endl;
                return false;
            }

            return true;
        }

        void execute() {
            for (; _program_counter < _source_size; _program_counter++) {
                execute_statement(static_cast<char>(_source[_program_counter]));
            }
        }

    private:
        void execute_statement(const char &statement) {
            switch (statement) {
                case '+':
                    _memory[_memory_pointer] ++;
                    break;
                
                case '-':
                    _memory[_memory_pointer] --;
                    break;
                
                case '>':
                    _memory_pointer ++;
                    break;
                
                case '<':
                    _memory_pointer --;
                    break;
                
                case '.':
                    std::cout << static_cast<char>(_memory[_memory_pointer]);
                    break;
                
                case ',':
                    _memory[_memory_pointer] = static_cast<uint8_t>(_getch());
                    break;
                
                case ':':
                    handle_copy_right();
                    break;
                
                case ';':
                    handle_copy_left();
                    break;
                
                case '*':
                    handle_doubling();
                    break;
                
                case '/':
                    handle_halfing();
                    break;
                
                case '[':
                    handle_loop_start();
                    break;
                
                case ']':
                    handle_loop_end();
                    break;
                    
                default:
                    break;
            }
        }

        void handle_loop_start() {
            if (_memory[_memory_pointer] != 0) return;
            auto it = _loop_map.find(_program_counter);
            if (it == _loop_map.end()) {
                std::cerr << "Missing ']' for '[' at position " << _program_counter << std::endl;
                _program_counter = _source_size;
                return;
            }
            _program_counter = it->second;
        }

        void handle_loop_end() {
            auto it = _loop_map.find(_program_counter);
            if (it == _loop_map.end()) {
                std::cerr << "Missing '[' for ']' at position " << _program_counter << std::endl;
                _program_counter = _source_size;
                return;
            }
            _program_counter = it->second - 1;
        }

        void handle_copy_right() {
            if (!_use_special) return;
            uint8_t origin = _memory[_memory_pointer];
            _memory_pointer ++;
            _memory[_memory_pointer] = origin;
        }

        void handle_copy_left() {
            if (!_use_special) return;
            uint8_t origin = _memory[_memory_pointer];
            _memory_pointer --;
            _memory[_memory_pointer] = origin;
        }

        void handle_doubling() {
            if (!_use_special) return;
            uint8_t origin = _memory[_memory_pointer];
            _memory[_memory_pointer] = origin * 2;
        }

        void handle_halfing() {
            if (!_use_special) return;
            uint8_t origin = _memory[_memory_pointer];
            _memory[_memory_pointer] = origin / 2;
        }

        std::string _source;
        size_t _source_size;
        size_t _program_counter = 0;
        uint8_t* _memory = new uint8_t[BRAINFUCK_THREAD_ARRAY_SIZE]();
        uint16_t _memory_pointer = 0;
        std::unordered_map<size_t, size_t> _loop_map;
        bool _use_special;
};

bool search_element(const char *haystack[], int haystack_size, const char *needle) {
    for (size_t i = 0; i < haystack_size; i++) {
        if (strcmp(haystack[i], needle) == 0) return true;
    }
    return false;
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        std::cerr << "Use: " << argv[0] << " <filename>" << std::endl;
        return -1;
    }

    bool use_special = search_element(argv, argc, "--use-special");

    std::ifstream source_file_input_stream(argv[1]);
    if (!source_file_input_stream) {
        std::cerr << "Failed to open File at " << argv[1] << std::endl;
        return -1;
    }

    std::string source{
        std::istreambuf_iterator<char>(source_file_input_stream),
        std::istreambuf_iterator<char>()
    };

    BrainFuckThread current_thread(source, use_special);
    if (!current_thread.lex()) return -1;
    current_thread.execute();
    return 0;
}