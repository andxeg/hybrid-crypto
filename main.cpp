#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <tuple>
#include "NTL/ZZ.h"

using namespace std;
using namespace NTL;

std::map<char, uint> hexNumber;

typedef std::pair<NTL::ZZ, NTL::ZZ> KEY_PAIR;//<private_key, public_key>

NTL::ZZ readHexNumber(fstream & inputFile) {
	NTL::ZZ p;
	std::string fileLine;
	std::getline(inputFile, fileLine);
	for (size_t i = 0; i < fileLine.size(); i++) {
		char c = fileLine[i];
		if (isdigit(c))
			p = 16*p + (c - '0');
		else
			p = 16*p + hexNumber[c];
	}
	return p;
}

bool checkParameters(const NTL::ZZ & p, const NTL::ZZ & q, const NTL::ZZ & a) {

	if ( (p - 1) % q != 0) {
		std::cout << "(p - 1)(mod q) != 0" << std::endl;
		return false;
	}

	if (NTL::PowerMod(a, q, p) != 1) {
		std::cout << "a^q(mod p) != 1" << std::endl;
		return false;
	}

	return true;
}

KEY_PAIR generateKeyPair(const NTL::ZZ & privateKey, const NTL::ZZ & a, const NTL::ZZ & p) {
	KEY_PAIR result;//<privateKey, a^privateKey mod p>
	NTL::ZZ publicKey = NTL::PowerMod(a, privateKey, p);
	return std::make_pair(privateKey, publicKey);
}


NTL::ZZ gostR3411(const NTL::ZZ & input) {
	NTL::ZZ result;
	return result;
}

NTL::ZZ generateKEK(const NTL::ZZ & privateKey, const NTL::ZZ & publicKey, const NTL::ZZ & p) {
	NTL::ZZ k = PowerMod(publicKey, privateKey, p);
	NTL::ZZ kek = gostR3411(k);
	return kek;
}

bool checkKeyPair(KEY_PAIR pair, const NTL::ZZ & a, const NTL::ZZ & p) {
	if (pair.second % p == a % p)
		return false;

	return true;
}


int main(int argc, char * argv[] ) {
	NTL::ZZ p, q, a, x, y;
	p = q = a = x = y = 0;
	hexNumber['A'] = 10;
	hexNumber['B'] = 11;
	hexNumber['C'] = 12;
	hexNumber['D'] = 13;
	hexNumber['E'] = 14;
	hexNumber['F'] = 15;
	
	if (argc != 2) {
		std::cout << "Try " << argv[0] << " <parameters_file>"  << std::endl;
		return 1;
	}

	//=============================READ PARAMETERS=============================
	std::fstream inputFile;
	inputFile.open(argv[1]);

	//READ P Q A X Y
	p = readHexNumber(inputFile);
	q = readHexNumber(inputFile);
	a = readHexNumber(inputFile);
	x = readHexNumber(inputFile);
	y = readHexNumber(inputFile);
	if (!checkParameters(p, q, a)) {
		std::cout << "PARAMETERS ARE INCORRECT" << std::endl;
		return 1;
	}

	std::cout << "PARAMETERS ARE CORRECT" << std::endl;
	
	KEY_PAIR sender = generateKeyPair(x, a, p);
	KEY_PAIR recipient = generateKeyPair(y, a, p);

	if (!checkKeyPair(sender, a, p) || 
		!checkKeyPair(recipient, a, p) ||
		NTL::PowerMod(sender.second, recipient.first, p) !=  
		NTL::PowerMod(recipient.second, sender.first, p) ) {
		std::cout << "KEYS ARE INCORRECT" << std::endl;
		return 1;
	}

	std::cout << "KEYS ARE CORRECT" << std::endl;


	//SEND_MESSAGE_FROM_X_TO_Y
	NTL::ZZ kek = generateKEK(sender.first, recipient.second, p);
	


	return 0;
}