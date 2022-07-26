#include <openssl/sha.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <vector>
#include <math.h>
#include <random>
#include <numeric>

using namespace std;

class KeyPair {
private:
	uint64_t privateKey;
	uint64_t publicKey;

public:
	KeyPair genKeyPair(KeyPair& key) {

		genPrivateKey(privateKey);
		genPublicKey(publicKey, privateKey);

		return key;
	}

	uint64_t getPrivateKey() {
		return privateKey;
	}

	uint64_t getPublicKey() {
		return publicKey;
	}

	void genPrivateKey(uint64_t& privkey) {

		mt19937_64 gen(random_device{}());
		uint64_t dec = gen() % 0xfffffffffffffff + 0x0;
		privkey = dec;
	}

	void genPublicKey(uint64_t& pubkey, uint64_t privkey) {

		mt19937_64 gen(random_device{}());
		uint64_t dec = gen() % 0xfffffffffffffff + 0x0;

		if (dec > privkey) {
			pubkey = dec - privkey;
		}
		else {
			pubkey = privkey - dec;
		}
	}

	void stringToHex(uint64_t dec) {

		string val[16] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"};
		string hex;

		while (dec != 0) {
			hex.insert(0, val[dec % 16]);
			dec /= 16;
		}

		int bytes = hex.length() / 2;
		int left = 0;
		int right = hex.length() - 1;

		for (int i = 0; i < bytes / 2; i++) {
			swap(hex[left], hex[static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(right) - 1]);
			swap(hex[right], hex[static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(left) + 1]);
			left += 2; right -= 2;
		}
		cout << hex << endl;
	}

	/*void printHex() {
		cout << "Private key: 0x";
		stringToHex(privateKey);
		cout << "Public key: 0x";
		stringToHex(publicKey);
	}

	void printKeys() {
		cout << "Private key: " << privateKey << '\n';
		cout << "Public key: " << publicKey << '\n';
	}*/
};

class Signature {
private:
	int message;

public:
	int signData(int mess, uint64_t key) {
		//2 random prime numbers
		double p = 11;
		double q = 22;
		double n = p * q;//calculate n
		uint64_t track;
		double phi = (p - 1) * (q - 1);//calculate phi

		double e = key;
		//for checking that 1 < e < phi(n) and gcd(e, phi(n)) = 1; i.e., e and phi(n) are coprime.
		while (e < phi) {
			track = gcd(e, phi);
			if (track == 1)
				break;
			else
				e++;
		}
		uint64_t c = pow(mess, e); //encrypt the message
		c = fmod(c, n);

		message = mess;

		return c;
	}

	bool verifySignature(int sign, int mess, uint64_t key) {
		//2 random prime numbers
		double p = 11;
		double q = 22;
		double n = p * q;//calculate n
		uint64_t track;
		double phi = (p - 1) * (q - 1);//calculate phi

		double e = key;
		//for checking that 1 < e < phi(n) and gcd(e, phi(n)) = 1; i.e., e and phi(n) are coprime.
		while (e < phi) {
			track = gcd(e, phi);
			if (track == 1)
				break;
			else
				e++;
		}

		double d1 = 1 / e;
		double d = fmod(d1, phi);
		double c = pow(mess, e); //encrypt the message
		uint64_t m = pow(c, d);
		m = fmod(m, n);

		if (m == sign && mess == message)
		{
			//cout << "\nTrue" << endl;
			return true;
		}
		else
		{
			//cout << "\nFalse" << endl;
			return false;
		}
	}

	/*void printSignature() {
		cout << "Signature: " << signData << '\n';
	}

	void printVSignature() {
		cout << "Verifying signature: " << verifySignature << '\n';
	}*/
};

class Account {
private:
	uint64_t accountID;
	uint64_t wallet[80];
	int balance = 0;
	int obj = 0;
	int coin = 0;

public:
	Account genAccount(Account& acc) {
		KeyPair key{};
		key.genKeyPair(key);
		//key.printHex();

		accountID = key.getPublicKey();
		wallet[obj] = key.getPublicKey();
		wallet[obj + 1] = key.getPrivateKey();

		obj += 2;

		return acc;
	}

	uint64_t getPub(int index) {
		return wallet[index];
	}

	uint64_t getID() {
		return accountID;
	}

	void addKeyPairToWallet() {
		KeyPair key{};
		key.genKeyPair(key);

		wallet[obj] = key.getPublicKey();
		wallet[obj + 1] = key.getPrivateKey();

		obj += 2;
	}

	void updateBalance(int& balance) {
		balance += coin;
		coin = 0;
	}

	void createPaymentOp(Account adressee, int quantity, int obj) {
		balance -= quantity;
		adressee.coin = quantity;
		updateBalance(adressee.balance);
	}

	int getBalance() {
		return balance;
	}

	void printBalance() {
		cout << "Balance: " << balance << endl;
	}

	int signData(int mess, int obj) {
		Signature signature{};
		int sign = signature.signData(mess, wallet[obj * 2 - 1]);

		return sign;
	}

};

class Operation {
private:
	Account sender; //отправитель
	Account receiver; //получатель
	int amount; //сколько
	int signature; //подпись отправителя

public:
	Operation createOperation(Account sender, Account receiver, int amount, int signature) {
		Operation operation;
		operation.sender = sender;
		operation.receiver = receiver;
		operation.amount = amount;
		operation.signature = signature;

		if (&Operation::verifyOperation) {
			operation.sender.createPaymentOp(operation.receiver, operation.amount, 1);
		}
		return operation;
	}

	bool verifyOperation(Operation operation) {

		Signature sign{};

		if (sign.verifySignature(operation.signature, operation.sender.getPub(0), 16)) {

			if (operation.amount <= operation.sender.getBalance()) {

				return true;

			}

		}

	}

	/*
	
	
	*/

};

class Transaction {
private:
	Operation setOfOperations[0xfffff];
	string transactionID;
	int nonce = 0;

public:
	Transaction createTransaction(Operation operation) {

		Transaction transaction;

		transaction.setOfOperations[nonce] = operation;
		transaction.nonce++;

		return transaction;
	}
};

class Hash {
private:
	unsigned char digest[SHA_DIGEST_LENGTH];
	string hash_str;
	string hash_data;

public:
	string toSHA1(string data) {
		//char* string[] = hash_str;
		SHA1((unsigned char*)&string, strlen(string), (unsigned char*)&digest);
		return hash_data;
	}
};

class Block {
private:
	Transaction setOfTransactions[0xff];
	string BlockID;
	string prevHash;
	
public:
	Block createBlock(Transaction setOfTransactions[], string prevHash) {
		Block block;
		string temp;
		Hash hash;
		BlockID = hash.toSHA1(temp);

		block.prevHash = prevHash;

		for (int i = 0; i < 0xff; i++)
		{
			block.setOfTransactions[i] = setOfTransactions[i];
		}

		return block;
	}

	Block genBlock(Transaction ttn) {
		Block block;
		string temp;;
		Hash hash;
		BlockID = hash.toSHA1(temp);

		block.setOfTransactions[0] = ttn;

		return block;
	}

	string getPrevHash() {
		return prevHash;
	}

	string getBlockID() {
		return BlockID;
	}

};

class Blockchain {
private:
	Account coinDatabase[210000];
	Block blockHistory[210];
	vector<Block> txDatabase;
	int faucetCoins = 1000;
	int index_b = 0;
	int index_a = 0;

public:
	Blockchain initBlockchain(Transaction ttn) {
		Block block;
		block.genBlock(ttn);

		blockHistory[index_b] = block;

		index_b++;
	}

	void getTokenFromFaucet(Account& account, int amount) {
		if (amount <= faucetCoins)
		{
			faucetCoins -= amount;
			int balance = account.getBalance() + amount;
			account.getBalance(balance);
		}
	}

	void validateBlock(Block block) {
		if (block.getPrevHash() == blockHistory[index_b - 1].getBlockID())
		{
			blockHistory[index_b] = block;
			index_b++;
		}
	}

	void setAccount(Account a) {
		coinDatabase[index_a] = a;
		index_a++;
	}

	void getTokenFromFaucet() {
		for (int i = 0; i < index_a; i++)
		{
			cout << "Your Account ID: " << coinDatabase[i].getID() << '\n'; 
			cout << "Your Balance: " << coinDatabase[i].getBalance() << '\n';
		}
	}

};
