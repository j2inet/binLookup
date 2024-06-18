using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text.RegularExpressions;
using System.Collections.Generic;
using System.Collections.Specialized;

namespace BinLookup.net
{
    internal class Program
    {

        const int MIN_ENTRY_LENGTH = 8;
        public static Stream ReadResource(String folder, String filename)
        {
            return ReadResource(Assembly.GetCallingAssembly(), folder, filename);
        }
        public static Stream ReadResource(Assembly assembly, string folder, string fileName)
        {
            string resourcePath;
            var assemblyName = assembly.GetName().Name;
            if (folder != null)
                resourcePath = $"{assemblyName}.{folder}.{fileName}";
            else
                resourcePath = $"{assemblyName}.{fileName}";

            return assembly.GetManifestResourceStream(resourcePath);
        }

        static void Main(string[] args)
        {
            var resourceStream = ReadResource("resources","binlist.csv");
            Regex numericPrefixExpression = new Regex(@"^\d+$");
            Regex numericRangeExpression = new Regex(@"^(\d+)-(\d+)$");
            List<string> prefixList = new List<string>();
            StringDictionary bankList = new StringDictionary();

            if(args.Length == 0)
            {
                Console.WriteLine("Usage: binlookup <credit card prefix/>");
                return;
            }

            var minLength = MIN_ENTRY_LENGTH;
            foreach(var prefix in args)
            {
                if(!numericPrefixExpression.IsMatch(prefix))
                {
                    Console.WriteLine("Invalid credit card prefix");
                    return;
                }

                if(prefix.Length < minLength)
                    minLength = prefix.Length;
            }
            if(minLength < MIN_ENTRY_LENGTH)
            {
                Console.WriteLine($"Credit card prefix must be at least {MIN_ENTRY_LENGTH} digits long");
                return;
            }


            int minPrefix = 6;
            int maxPrefix = 6; 
            using (StreamReader stream = new StreamReader(resourceStream))
            {
                while(!stream.EndOfStream)
                {
                    var line = stream.ReadLine().Trim();
                    if(string.IsNullOrEmpty(line))
                        continue;
                    if(line.StartsWith("#"))
                        continue;
                    var parts = line.Split(",,");
                    if(parts.Length != 2)
                        continue;
                    var range = parts[0].Trim();
                    var bankName = parts[1].Trim();

                    if(numericPrefixExpression.IsMatch(range))
                    {
                        if (range.Length < minPrefix)
                        {
                            minPrefix = range.Length;
                        }
                        if(range.Length > maxPrefix)
                        {
                            maxPrefix = range.Length;
                        }
                        bankList[range] = bankName;
                    }
                    else if (numericRangeExpression.IsMatch(range))
                    {
                        var match = numericRangeExpression.Match(range);
                        var m1 = match.Groups[1].Value;
                        var m2 = match.Groups[2].Value;
                        if (m1.Length != m2.Length)
                            throw new Exception("Invalid range");
                        if (m1.Length < minPrefix)
                        {
                            minPrefix = m1.Length;
                        }
                        if (m1.Length > maxPrefix)
                        {
                            maxPrefix = m1.Length;
                        }
                        var start = int.Parse(m1);
                        var end = int.Parse(m2);
                        for(int i = start; i <= end; i++)
                        {
                            prefixList.Add(i.ToString());
                            bankList[i.ToString()] = bankName;
                        }
                    }
                    else
                    {
                        
                    }


                }
            }

            foreach(string cardPrefix in args)
            {
                if(bankList.ContainsKey(cardPrefix))
                {
                    Console.WriteLine($"Card prefix {cardPrefix} belongs to {bankList[cardPrefix]}");
                    continue;
                }
                for(var prefixLength = maxPrefix; prefixLength >= minPrefix; prefixLength--)
                {
                    var prefix = cardPrefix.Substring(0, prefixLength);
                    if(bankList.ContainsKey(prefix))
                    {
                        Console.WriteLine($"Card prefix {cardPrefix} belongs to {bankList[prefix]}");
                        break;
                    }
                }
            }
        }
    }
}
