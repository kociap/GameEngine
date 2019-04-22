#include "utils/simple_xml_parser.hpp"

#include <cctype>

namespace utils {
    namespace xml {
        enum class State { tag_open, tag_name, tag_name_end, tag_close, property_name, property_name_end, property_value, property_value_end };

        static void parse_tag(std::string const& str, std::string::size_type& i, containers::Vector<Tag>& tags) {
            Tag tag;
            tag.type = Tag_Type::opening;
            State state = State::tag_open;
            for (i; i < str.size(); ++i) {
                char c = str[i];
                if (c == '>')
                    break;

                if (state == State::tag_open) {
                    if (c == '/') {
                        tag.type = Tag_Type::closing;
                    } else {
                        state = State::tag_name;
                        tag.name += c;
                    }
                } else if (state == State::tag_name) {
                    if (std::isalnum(c)) {
                        tag.name += c;
                    } else {
                        if (c == '/') {
                            tag.type = Tag_Type::self_closing;
                            state = State::tag_close;
                        } else {
                            state = State::tag_name_end;
                        }
                    }
                } else if (state == State::tag_name_end) {
                    if (c == '/') {
                        tag.type = Tag_Type::self_closing;
                        state = State::tag_close;
                    } else if (std::isalnum(c)) {
                        tag.properties.emplace_back();
                        tag.properties.back().name += c;
                        state = State::property_name;
                    }
                } else if (state == State::property_name) {
                    if (std::isalnum(c)) {
                        tag.properties.back().name += c;
                    } else {
                        state = State::property_name_end;
                    }
                } else if (state == State::property_name_end) {
                    if (c == '"') {
                        state = State::property_value;
                    }
                } else if (state == State::property_value) {
                    if (c == '"') {
                        state = State::property_value_end;
                    } else {
                        tag.properties.back().value += c;
                    }
                } else if (state == State::property_value_end) {
                    if (c == '/') {
                        tag.type = Tag_Type::self_closing;
                        state = State::tag_close;
                    } else if (std::isalnum(c)) {
                        tag.properties.emplace_back();
                        tag.properties.back().name += c;
                        state = State::property_name;
                    }
                }
            }
            tags.push_back(std::move(tag));
        }

        containers::Vector<Tag> parse(std::string const& str) {
            containers::Vector<Tag> tags;

            for (std::string::size_type i = 0; i < str.size(); ++i) {
                if (str[i] != '<') {
                    continue;
                } else {
                    parse_tag(str, ++i, tags);
                }
            }

            return tags;
        }
    } // namespace xml
} // namespace utils