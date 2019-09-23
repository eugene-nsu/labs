
#include <fstream>
#include <iostream>
#define EXT_COEF 1.25
#define BEG_ARRAY_POW_LEN 10
using namespace std;

class word {

private:
	word* next;
	char* word_value;
	unsigned char word_len;
	unsigned int word_count;
	unsigned int hash;

public:
	word(char*&& WORD_VALUE, unsigned char WORD_LEN, const unsigned int& Q, unsigned int*& powx) {
		word_count = 1;
		word_len = WORD_LEN;
		word_value = new char[WORD_LEN];
		hash = 0;
		next = NULL;

		for (int i = 0; i < WORD_LEN; i++) {
			word_value[i] = WORD_VALUE[i];
			hash += WORD_VALUE[i] * powx[WORD_LEN - i - 1];
			hash = hash % Q;
		}


	}
	void set_poitner_to_next(class word*& curent_pointer, class word*& next_pointer) {
		curent_pointer->next = next_pointer;
		curent_pointer = next_pointer;
	}
	void add_word_count() {
		word_count += 1;
	}
	char compare(unsigned int& second_hash, char*& second_word, unsigned char& second_word_len) {
		if (hash == second_hash) {
			if (word_len == second_word_len) {
				bool t = 0;
				for (unsigned char i = 0; i < word_len; i++) {
					if (word_value[i] != second_word[i]) t = 1;
				}
				if (0 == t) return 1;
			}
		}
		return 0;
	}

	char is_next() {
		if (next != NULL) {
			return 1;
		}
		else {
			return 0;
		}
	}

	word* get_pointer_to_next() {
		return next;
	}
	char* get_word() {
		return word_value;
	}
	unsigned char get_word_len() {
		return word_len;
	}
	unsigned int get_word_count() {
		return word_count;
	}

};

void ext_powx(unsigned int** powx, unsigned char& max_pow);
char add_or_make(word* & root, char *&& word, unsigned char word_len, unsigned int*& powx, unsigned char& max_pow, const unsigned int & Q);
char check_list(word * root, unsigned int & hash, char* &word, unsigned char &word_len);
void print_freq_table(word * word, unsigned int & word_count, ofstream & out);




int main()
{
	ifstream in("in.txt", ios_base::binary);
	if (!in.is_open()) {
		cout << "create in.txt" << endl;
		exit(0);
	}
	unsigned long text_len;

	in.seekg(0, in.end);
	text_len = (unsigned long)in.tellg();
	char* text = new char[text_len];
	if (0 == text_len) {
		cout << "in.txt is empty" << endl;
		exit(0);
	}
	in.seekg(0, in.beg);
	in.read(text, text_len * sizeof(char));

	ofstream out("out.txt", ios_base::out);
	const unsigned int Q = 2500000043;

	const unsigned char X = 2;
	unsigned int* powx = new unsigned int[BEG_ARRAY_POW_LEN];

	unsigned char max_pow = BEG_ARRAY_POW_LEN;
	powx[0] = X;

	for (int i = 1; i < BEG_ARRAY_POW_LEN; i++) {
		powx[i] = powx[i - 1] * X;
	}

	unsigned long prev_spec_pos = 0;
	word* root = NULL;

	for (unsigned int i = 0; i < text_len; i++) {
		if ((' ' == text[i]) || ('\n' == text[i]) || ('\r' == text[i])) {

			if (prev_spec_pos != i) {
				word* obj = new word(&text[prev_spec_pos], (unsigned char)(i - prev_spec_pos), Q, powx);
				root = obj;
				prev_spec_pos = i + 1;
				break;
			}

			prev_spec_pos = i + 1;

		}
	}
	unsigned int word_count = 1;
	word* previous_pointer = root;

	for (unsigned int i = prev_spec_pos; i < text_len; i++) {
		if ((' ' == text[i]) || ('\n' == text[i]) || ('\r' == text[i])) {

			if (prev_spec_pos != i) {
				word_count++;
				if (!add_or_make(root, &text[prev_spec_pos], (unsigned char)(i - prev_spec_pos), powx, max_pow, Q)) {
					word* obj = new word(&text[prev_spec_pos], (unsigned char)(i - prev_spec_pos), Q, powx);
					obj->set_poitner_to_next(previous_pointer, obj);
				}
			}
			prev_spec_pos = i + 1;
		}
	}

	if (prev_spec_pos != text_len) {
		word_count++;
		if (!add_or_make(root, &text[prev_spec_pos], (unsigned char)(text_len - prev_spec_pos), powx, max_pow, Q)) {
			word* obj = new word(&text[prev_spec_pos], (unsigned char)(text_len - prev_spec_pos), Q, powx);
			obj->set_poitner_to_next(previous_pointer, obj);
		}
	}

	print_freq_table(root, word_count, out);

	return 0;
}

void ext_powx(unsigned int** powx, unsigned char& max_pow)
{
	unsigned char previous_pow = max_pow;
	max_pow = (unsigned char)(max_pow * EXT_COEF);
	unsigned int* ext_powx = new unsigned int[max_pow];
	
	for (int i = 0; i < previous_pow; i++) {
		ext_powx[i] = *powx[i];
	}
	for (int i = previous_pow; i < max_pow; i++) {
		ext_powx[i] = ext_powx[i - 1] * ext_powx[0];
	}
	delete* powx;
	*powx = ext_powx;

}


char add_or_make(word* & root, char *&& word, unsigned char word_len, unsigned int*& powx, unsigned char& max_pow, const unsigned int & Q) {
	if (word_len > max_pow) {
		ext_powx(&powx, max_pow);
	}
	unsigned int hash = 0;

	for (int i = 0; i < word_len; i++) {
		hash += word[i] * powx[word_len - i - 1];
		hash = hash % Q;
	}
	return (check_list(root, hash, word, word_len));
}

char check_list(word * root, unsigned int &hash, char*& word, unsigned char& word_len) {
	if (root->compare(hash, word, word_len)) {
		root->add_word_count();
		return 1;
	}
	else if (root->is_next()) {
		return (check_list(root->get_pointer_to_next(), hash, word, word_len));
	}
	else {
		return 0;
	}

}

void print_freq_table(word * word, unsigned int& word_count, ofstream& out) {
	out.write(word->get_word(), word->get_word_len() * sizeof(char));
	out << ' ' << word->get_word_count() << ' ' << ((double)word->get_word_count() / word_count * 100) << '%' << endl;
	if (word->is_next()) {
		print_freq_table(word->get_pointer_to_next(), word_count, out);
	}
	else {
		out << "word count= " << word_count;
	}
}
//test git commit