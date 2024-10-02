#include <iostream>
#include <string>

using namespace std;


// Функция перевода из двоичной ПСС в десятичную ПСС
int convert(const string& str) 
{
	int sum = 0;
	int strLen = str.size();

	for (int i = 0; i < strLen; i++) 
	{
		int discharge = strLen - 1 - i;
		sum += (str[i] -'0') * pow(2, discharge);
	}

	return sum;
}


// Основная нить дочернего процесса.
int main(int argc, char* argv[])
{
	cout << "In process provided args: " << argc << endl;

	char* arg = argv[0];
	string str{ arg };

	int result = convert(str);

	cout << "Result is: " << result << endl;

	system("pause");
	return 0;
}
