#ifndef VDF_HPP_
#define VDF_HPP_

#include <filesystem>
#include <map>
#include <string>
#include <vector>

class VDF
{
    public:
        VDF() noexcept;

        void LoadFromFile(std::filesystem::path const &path, bool append = false);
        void LoadFromText(std::string const &text, bool append = false);

        std::vector<std::string> GetValuesWithFilter(std::string const &filter);

        std::map<std::string, std::string> KeyValue;

    private:
        void AddKeyValuePair();
        std::string RemoveWhitespaces(std::string const &text);
        void ParseVDF(std::string const &text);
        void ProcessChar(char c);
        void LookForKey(char c);
        void LookForValue(char c);
        void ReadKey(char c);
        void ReadValue(char c);

        enum class VDFState
        {
            LookingForKey,
            LookingForValue,
            ReadingKey,
            ReadingValue,
            QuoteOrBracketExpectedError,
            MissingBracketAtEndError
        };
        VDFState state_;
        std::string key_;
        std::string value_;
        std::vector<std::string> hierarchy_;
};

#endif
