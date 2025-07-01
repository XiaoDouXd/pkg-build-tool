using System.Text;

namespace binary_generator;

internal static class Program
{
    private static string FormatName(string input)
    {
        // 把文件名转换成 C 风格标识符
        var result = new StringBuilder();
        bool lastWasUnderscore;

        // 处理第一个字符（需确保不以数字开头）
        var firstChar = input[0];
        if (char.IsLetter(firstChar) || firstChar == '_')
        {
            result.Append(firstChar);
            lastWasUnderscore = (firstChar == '_');
        }
        else if (char.IsDigit(firstChar))
        {
            result.Append('_').Append(firstChar);
            lastWasUnderscore = false;
        }
        else // 无效首字符
        {
            result.Append('_');
            lastWasUnderscore = true;
        }

        // 处理剩余字符
        for (var i = 1; i < input.Length; i++)
        {
            var c = input[i];
            if (char.IsLetterOrDigit(c) || c == '_')
            {
                result.Append(c);
                lastWasUnderscore = (c == '_');
            }
            else if (!lastWasUnderscore) // 替换无效字符（不创建连续下划线）
            {
                result.Append('_');
                lastWasUnderscore = true;
            }
        }
        return result.ToString();
    }

    private static string BinToCpp(byte[] bytes, int offset, int len, string name, string outputPath, int idx)
    {
        var isWhole = offset == 0 && len == bytes.Length;
        var srcName = isWhole ? $"RES_{name}" : $"RES{idx}_{name}";

        // ReSharper disable once UseRawString
        var cpp = new StringBuilder($@"#include <array>
#include ""{srcName}.h""

namespace RC {{
    const std::array<const uint8_t, {len + 1}> __{srcName} = {{ ");
        for (var i = 0; i < len; i++)
        {
            cpp.Append($"0x{bytes[i + offset]:x2}, ");
        }
        cpp.Append("0x00, ");
        cpp.Append("};\n");
        cpp.Append(
            $"    const std::span<const uint8_t, {len}> {srcName}(__{srcName}.data(), {len});\n"
            );
        cpp.Append('}');

        // ReSharper disable once UseRawString
        var header = new StringBuilder($@"#pragma once

#include <span>
#include <cstdint>

namespace RC {{
    extern const std::span<const uint8_t, {len}> {srcName};
}}");

        if (!Directory.Exists(outputPath)) Directory.CreateDirectory(outputPath);
        File.WriteAllText($"{outputPath}/{srcName}.h", header.ToString());
        File.WriteAllText($"{outputPath}/{srcName}.cpp", cpp.ToString());
        return srcName;
    }

    private static void CreateMap(IReadOnlyList<string> srcNames, string name, string outputPath)
    {
        var srcName = $"RES_{name}";
        // ReSharper disable once UseRawString
        var cpp = new StringBuilder($@"#include ""{srcName}.h""");
        foreach (var src in srcNames) cpp.Append($"\n#include \"{src}.h\"");
        cpp.Append($@"
namespace RC {{
    extern const std::array<std::span<const uint8_t>, {srcNames.Count}> {srcName} = {{
");
        foreach (var src in srcNames) cpp.Append($"        std::span<const uint8_t>({src}),\n");
        cpp.Append("    };\n}");
        // ReSharper disable once UseRawString
        var header = new StringBuilder($@"#pragma once

#include <array>
#include <span>
#include <cstdint>

namespace RC {{
    extern const std::array<std::span<const uint8_t>, {srcNames.Count}> {srcName};
}}");
        if (!Directory.Exists(outputPath)) Directory.CreateDirectory(outputPath);
        File.WriteAllText($"{outputPath}/{srcName}.h", header.ToString());
        File.WriteAllText($"{outputPath}/{srcName}.cpp", cpp.ToString());
    }

    private static void FileToCpp(string path)
    {
        if (!File.Exists(path))
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine($"File {path} does not exist");
            Console.ResetColor();
        }

        var io = path.Split(':');
        if (io.Length <= 0)
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine($"Error: Binary generator is missing required bytes, file: {path}");
            Console.ResetColor();
        }

        var input = io[0];
        var output = io.Length < 2 ? Path.GetDirectoryName(input) ?? "./" : io[1];
        if (string.IsNullOrWhiteSpace(output)) output = "./";

        var bytes = File.ReadAllBytes(input);
        if (bytes is not { Length: > 0 })
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine($"Error: Binary generator is missing required bytes, file: {path}");
            Console.ResetColor();
        }

        var srcList = new List<string>();
        const int maxBlockSize = 4 * 1024 * 1024;
        var offset = 0;
        var name = FormatName(Path.GetFileNameWithoutExtension(input));
        var idx = 0;
        while (bytes.Length - offset > maxBlockSize) // 4mib
        {
            srcList.Add(BinToCpp(bytes, offset, maxBlockSize, name, output, idx++));
            offset += maxBlockSize;
        }
        if (bytes.Length - offset > 0) srcList.Add(BinToCpp(bytes, offset, bytes.Length - offset, name, output, idx));
        if (srcList.Count > 1) CreateMap(srcList, name, output);
    }

    private static void Main(string[] args)
    {
        try
        {
            foreach (var paths in args) FileToCpp(paths);
        }
        catch (Exception e)
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine(e);
        }
    }
}