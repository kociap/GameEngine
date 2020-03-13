#include <component_header_parser.hpp>
#include <tuple>
#include <unordered_map>

// identifier -> identifier identifier_allowed_character | non_number
// opt_attributes -> opt_attributes identifier | identifier
// class_declaration -> class opt_attributes identifier : access_specifier identifier { class_body };
namespace anton_engine {
    static bool is_whitespace(char c) {
        return c == '\n' || c == '\t' || c == ' ';
    }

    static bool is_digit(char c) {
        return c >= 48 && c < 58;
    }

    static bool is_alpha(char c) {
        return (c >= 97 && c < 123) || (c >= 65 && c < 91);
    }

    static bool is_allowed_identifier_character(char c) {
        return c == '_' || is_digit(c) || is_alpha(c);
    }

    enum class Token {
        keyword_class,
        identifier,
        opening_brace,
        macro_component,
    };

    struct Word {
        std::string string;
        Token token;
    };

    class Lexer {
    public:
        std::unordered_map<std::string, Token> tokens;

        Lexer();

        atl::Vector<Word> parse(File&);

    private:
        std::tuple<File::iterator, Token, std::string> scan(File::iterator begin, File::iterator end);
    };

    Lexer::Lexer(): tokens{{"class", Token::keyword_class}, {"{", Token::opening_brace}, {"COMPONENT", Token::macro_component}} {}

    atl::Vector<Word> Lexer::parse(File& file) {
        atl::Vector<Word> tokens_vec;
        for (File::iterator current = file.begin(), end = file.end(); current != end;) {
            auto [new_iter, token_type, string] = scan(current, end);
            current = new_iter;
            tokens_vec.emplace_back(string, token_type);
        }
        return tokens_vec;
    }

    std::tuple<File::iterator, Token, std::string> Lexer::scan(File::iterator begin, File::iterator const end) {
        char current = *begin;
        while (begin != end && is_whitespace(current)) {
            current = *(++begin);
        }

        File::iterator copy = begin + 1;
        if (begin != end && is_allowed_identifier_character(*begin)) {
            while (copy != end && is_allowed_identifier_character(*copy)) {
                ++copy;
            }
        }

        std::string identifier(begin, copy);
        auto tokens_iter = tokens.find(identifier);
        if (tokens_iter != tokens.end()) {
            return {copy, tokens_iter->second, tokens_iter->first};
        } else {
            tokens.emplace(identifier, Token::identifier);
            return {copy, Token::identifier, identifier};
        }
    }

    atl::Vector<std::string> parse_component_header(File& file) {
        atl::Vector<std::string> class_names;
        Lexer lexer;
        auto tokens = lexer.parse(file);
        for (int64_t i = 0; i + 2 < tokens.size(); ++i) {
            if (tokens[i].token == Token::keyword_class) {
                if (tokens[i + 1].token == Token::macro_component && tokens[i + 2].token == Token::identifier) {
                    class_names.push_back(tokens[i + 2].string);
                }
                i += 2;
            }
        }
        return class_names;
    }
} // namespace anton_engine
