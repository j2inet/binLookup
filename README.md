# Bank Identification Number Lookup Example

C# and C++ example of identifying the bank from the card number. Only the first 8 credit card digits are needed. 
were about 6 banks from Canada and Estonia eliminated from the data source, then only the first 6 digits 
would be needed. 

## How to Bank Identification Numbers (BIN) Work?

Visa, MasterCard, and other card networks allocate ranges of numbers to banks and institutions. Generally,
the first 6 digits of the card number identify the bank issuer. Though there are some isntances of only
4 digits being needed and a few examples of 8 digits being needed.

## About the Data Source

The associations between credit card prefixes and banks is stored in a CSV file. For both the C# and 
C++ file, I've embedded the files into the executables as resources.  I found the data source for this
from the website of [Steve Morse](https://stevemorse.org/ssn/List_of_Bank_Identification_Numbers.html). 
He said that the data was once part of a Wikipedia article, but was removed. This information might be 
incomplete or outdated. It is only provided as an example. If you need a data source for a production 
environment, please look elsewhere. While this code is fine for an exmple, it is not to be relied upon
for something of commercial interest. 

