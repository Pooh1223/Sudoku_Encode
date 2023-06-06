#include <bits/stdc++.h>
using namespace std;

void remove_spaces(string& s);

void print_board(vector<vector<int>>& board);
void input_clear();

void encode(string& raw_sentence);
void decode();

string encrypt(string raw_sentence);
string shift(string s);
void split_to_chunk(vector<string>& v,string& cipher,int mx_size);
string cipher_string(string& sudoku_board,string& ciphertext,int number_to_remove);
string mix_strings(string alphabet_cipher,string sudoku_problem_board,int max_consecutive);

string decrypt(string ciphertext);
void sudoku_string_to_board(vector<vector<int>>& board,set<int>& highlight_num,string& sudoku_problem_board);
string choose_highlight_char(string& alphabet_cipher,vector<vector<int>>& board,set<int>& highlight_num);
string shift_back(string s);

void generate_sudoku();
bool valid_preboard(vector<vector<int>>& board,vector<int>& preset_pos);
bool is_board_filled(vector<vector<int>>& board);
bool sudoku_filler(vector<vector<int>>& board);
vector<int> valid_number(vector<vector<int>>& board,int& x,int& y);
bool unique_solution_check(vector<vector<int>>& board,int& counter);

int N = 3;
int sz = N * N;
int shift_cnt = 3;

int main(){
	srand(time(NULL));

	int encode_decode;
	cout << "/////////////////////////////////////////////////////////////////////////\n";
	cout << "///   if you want to encode enter 1 or decode enter 2: (0 to break)   ///\n";
	cout << "/////////////////////////////////////////////////////////////////////////\n";

	while(cin >> encode_decode){
		if(encode_decode == 1){
			cout << "Input your message: ";
			string raw_sentence = "";
			input_clear();

			getline(cin,raw_sentence);
			// input_clear();
			encode(raw_sentence);
		} else if(encode_decode == 2){
			decode();
		} else if(encode_decode == 0){
			cout << "End.\n";
			break;
		} else {
			cout << "Invalid.\n";
		}
		cout << "/////////////////////////////////////////////////////////////////////////\n";
		cout << "///   if you want to encode enter 1 or decode enter 2: (0 to break)   ///\n";
		cout << "/////////////////////////////////////////////////////////////////////////\n";
	} 
	
	return 0;
}

void encode(string& raw_sentence){
	// string raw_sentence = "";
	
	fstream file;
	// file.open("encode.txt",ios::in);

	// string input = "";
	// while(getline(file,input)){
	// 	raw_sentence += input;
	// }

	// file.close();
	
	remove_spaces(raw_sentence);

	string ciphertext = encrypt(raw_sentence);

	// write it back
	file.open("encode.txt",ios::out|ios::trunc);
	file << ciphertext;
}

void decode(){
	string raw_sentence = "";
	
	fstream file;
	file.open("decode.txt",ios::in);

	string input = "";
	while(getline(file,input)){
		raw_sentence += input;
	}

	file.close();

	string ciphertext = decrypt(raw_sentence);

	// write it back
	file.open("decode.txt",ios::out|ios::trunc);
	file << ciphertext;
}

string decrypt(string ciphertext){
	string cipher = "";
	string alphabet_cipher = "";
	string sudoku_problem_board = "";
	for(int i = 0;i < ciphertext.length();++i){
		if(isalpha(ciphertext[i])){
			alphabet_cipher += ciphertext[i];
		} else {
			sudoku_problem_board += ciphertext[i];
		}
		if(alphabet_cipher.length() == sz * sz){
			int pos = i + 1;
			// make sure all the message of this board is added
			while(pos < ciphertext.length() && !isalpha(ciphertext[pos])){
				sudoku_problem_board += ciphertext[pos++];
			}

			// cout << "board len:\n" << sudoku_problem_board.length() << " " << ciphertext.length() << "\n";
			// cout << "board:\n" << sudoku_problem_board << "\n";

			vector<vector<int>> board(sz,vector<int>(sz,0));
			set<int> highlight_num;
			
			sudoku_string_to_board(board,highlight_num,sudoku_problem_board);

			sudoku_filler(board);

			cipher += choose_highlight_char(alphabet_cipher,board,highlight_num);

			alphabet_cipher = "";
			sudoku_problem_board = "";
			i = pos - 1;
		}
	}

	string decipher = shift_back(cipher);
	cout << "decipher:\n" << decipher << "\n";

	return decipher;
}

string choose_highlight_char(string& alphabet_cipher,vector<vector<int>>& board,set<int>& highlight_num){
	string cipher = "";
	int cnt = 0;
	int len = 0;
	for(int i = 0;i < sz * sz;++i){
		int x = i / sz;
		int y = i % sz;
		if(highlight_num.count(board[x][y])){
			if(cnt < 2){
				// cout << "len alpha " << cnt << ":\n" << alphabet_cipher[i] << "\n";
				len += alphabet_cipher[i] - 'a';
				cnt++;
			} else {
				if(cipher.length() < len) cipher += alphabet_cipher[i];
				else break;
			}
		}
	}

	// cout << "calc len:\n" << len << "\n";

	return cipher;
}

void sudoku_string_to_board(vector<vector<int>>& board,set<int>& highlight_num,string& sudoku_problem_board){
	int i;
	for(i = 0;i < sz * sz;++i){
		int x = i / sz;
		int y = i % sz;
		board[x][y] = sudoku_problem_board[i] - '0';
	}
	for(;i < sudoku_problem_board.length();++i){
		highlight_num.insert(sudoku_problem_board[i] - '0');
	}
}

string encrypt(string raw_sentence){
	// cout << "before:\n" << raw_sentence << "\n";
	string cipher = shift(raw_sentence);
	// cout << "after:\n" << cipher << "\n";

	vector<string> split_cipher;
	split_to_chunk(split_cipher,cipher,50);

	string mixed = "";
	int max_consecutive = 3;

	for(int k = 0;k < split_cipher.size();++k){
		int number_to_remove = (split_cipher[k].length() + 2) % sz == 0 ? (split_cipher[k].length() + 2) / sz : (split_cipher[k].length() + 2) / sz + 1;

		generate_sudoku();
		
		fstream sudoku_board_file;
		string sudoku;

		sudoku_board_file.open("sudoku_board.txt",ios::in);
		getline(sudoku_board_file,sudoku);

		string alphabet_cipher = cipher_string(sudoku,split_cipher[k],number_to_remove);
		// cout << "middle:\n" << alphabet_cipher << "\n";
		string sudoku_problem_board;
		getline(sudoku_board_file,sudoku_problem_board);
		for(int i = 0;i < number_to_remove;++i){
			sudoku_problem_board += sudoku[i + sz * sz];
		}
		sudoku_board_file.close();

		mixed += mix_strings(alphabet_cipher,sudoku_problem_board,max_consecutive);
	}

	cout << "final:\n" << mixed << "\n";

	return mixed;
}

void split_to_chunk(vector<string>& v,string& cipher,int mx_size){
	string tmp = "";
	for(int i = 0;i < cipher.length();++i){
		tmp += cipher[i];
		if(tmp.length() == mx_size || i == cipher.length() - 1){
			v.push_back(tmp);
			tmp = "";
		}
	}
}

string mix_strings(string alphabet_cipher,string sudoku_problem_board,int max_consecutive){
	int pos1 = 0,pos2 = 0;

	string mixed = "";
	while(pos1 < alphabet_cipher.length() || pos2 < sudoku_problem_board.length()){
		if(pos1 >= alphabet_cipher.length()){
			mixed += sudoku_problem_board[pos2++];
		} else if(pos2 >= sudoku_problem_board.length()){
			mixed += alphabet_cipher[pos1++];
		} else {
			if(pos1 <= pos2){
				int move = min(int(alphabet_cipher.length() - pos1),rand() % max_consecutive + 1);
				for(int i = 0;i < move;++i){
					mixed += alphabet_cipher[i + pos1];
				}
				pos1 += move;
			} else {
				int move = min(int(sudoku_problem_board.length() - pos2),rand() % max_consecutive + 1);
				for(int i = 0;i < move;++i){
					mixed += sudoku_problem_board[i + pos2];
				}
				pos2 += move;
			}
		}
	}

	return mixed;
}

string cipher_string(string& sudoku_board,string& ciphertext,int number_to_remove){
	set<int> remove_numbers;
	while(remove_numbers.size() < number_to_remove){
		remove_numbers.insert(rand() % sz + 1);
	}

	// dicide first two letters ,add the number of them
	// will the length of ciphertext
	// i.e. az -> a(0) + z(25) = 25

	int mean = ciphertext.length() / 2;
	// cout << "ciphertext:\n" << ciphertext.length() << "\n";
	// cout << "mean:\n" << mean << "\n";
	char len[2];
	if(25 - mean == 0){
		len[0] = 'z';
		len[1] = 'z';
	} else if(25 - mean == 1){
		len[0] = 'z';
		len[1] = char(ciphertext.length() - 25 + 'a');
	} else {
		len[0] = char((mean - min(mean,rand() % (25 - mean))) + 'a');
		len[1] = char((ciphertext.length() - (len[0] - 'a')) + 'a');
	}

	// cout << "len:\n" << len[0] << " " << len[1] << "\n";

	int cnt = 0;
	string sudoku_cipher = "";
	for(int i = 0;i < sudoku_board.size();++i){
		if(remove_numbers.count(sudoku_board[i] - '0')){
			if(cnt < 2){
				sudoku_cipher += len[cnt];
			} else {
				if(cnt - 2 < ciphertext.length()) sudoku_cipher += ciphertext[cnt - 2];
				else {
					if(rand() % 2) sudoku_cipher += char(rand() % 26 + 'a');
					else sudoku_cipher += char(rand() % 26 + 'A');
				}
			}
			cnt++;
		} else {
			if(rand() % 2) sudoku_cipher += char(rand() % 26 + 'a');
			else sudoku_cipher += char(rand() % 26 + 'A');
		}
	}

	for(auto iter : remove_numbers){
		sudoku_board += char(iter + '0');
	}

	return sudoku_cipher;
}

string shift(string s){
	for(int i = 0;i < s.length();++i){
		if(s[i] - 'a' >= 0 && s[i] - 'a' < 26){
			s[i] = char((s[i] - 'a' + shift_cnt) % 26 + 'a');
		} else if(s[i] - 'A' >= 0 && s[i] - 'A' < 26){
			s[i] = char((s[i] - 'A' + shift_cnt) % 26 + 'A');
		}
	}
	return s;
}

string shift_back(string s){
	for(int i = 0;i < s.length();++i){
		if(s[i] - 'a' >= 0 && s[i] - 'a' < 26){
			s[i] = char((s[i] - 'a' - shift_cnt + 26) % 26 + 'a');
		} else if(s[i] - 'A' >= 0 && s[i] - 'A' < 26){
			s[i] = char((s[i] - 'A' - shift_cnt + 26) % 26 + 'A');
		}
	}
	return s;
}

void generate_sudoku(){
	vector<vector<int>> board(sz,(vector<int>(sz,0)));
	vector<int> nums(sz,0);
	for(int i = 0;i < nums.size();++i) nums[i] = i + 1;
	
	
	// print_board(board);
	sudoku_filler(board);

	// cout << "wow\n";
	print_board(board);

	fstream sudoku_board;
	string output_file_name = "sudoku_board.txt";
	sudoku_board.open(output_file_name,ios::out|ios::trunc);
	
	// answer
	for(int i = 0;i < board.size();++i){
		for(int j = 0;j < board[i].size();++j){
			sudoku_board << board[i][j];
		}
	}
	sudoku_board << "\n";

	vector<int> preset_pos(sz * sz,0);
	for(int i = 0;i < preset_pos.size();++i) preset_pos[i] = i;
	random_shuffle(preset_pos.begin(),preset_pos.end());

	for(int i = 0;i < preset_pos.size();++i){
		int x = preset_pos[i] / sz;
		int y = preset_pos[i] % sz;
		// int original_val = board[x][y];
		int counter = 0;

		vector<vector<int>> final_board(board);
		final_board[x][y] = 0;
		if(unique_solution_check(final_board,counter)){
			board[x][y] = 0;
		}
	}
	

	cout << "final\n";
	print_board(board);

	// problem
	for(int i = 0;i < board.size();++i){
		for(int j = 0;j < board[i].size();++j){
			sudoku_board << board[i][j];
		}
	}

	sudoku_board.close();
}

void print_board(vector<vector<int>>& board){
	cout << "Board:\n";
	for(int i = 0;i < board.size();++i){
		for(int j = 0;j < board[i].size();++j){
			cout << board[i][j] << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}

bool unique_solution_check(vector<vector<int>>& board,int& cnt){
	for(int i = 0;i < sz * sz;++i){
		int x = i / sz;
		int y = i % sz;

		if(board[x][y] == 0){ // not visited
			for(auto k : valid_number(board,x,y)){
				board[x][y] = k;
				if(is_board_filled(board)){
					cnt++;
					if(cnt > 1) return false;
				} else {
					if(!unique_solution_check(board,cnt)){
						return false;
					}
				}	
			}
			board[x][y] = 0;
			break;
		}
	}

	return true;
}

bool is_board_filled(vector<vector<int>>& board){
	for(int i = 0;i < board.size();++i){
		for(int j = 0;j < board[i].size();++j){
			if(board[i][j] == 0) return false;
		}
	}
	return true;
}

// int cnt = 0;

bool sudoku_filler(vector<vector<int>>& board){
	for(int i = 0;i < sz * sz;++i){
		int x = i / sz;
		int y = i % sz;
		// cnt++;

		if(board[x][y] == 0){ // not visited
			// if(cnt % 1000000 == 0) print_board(board);
			for(auto k : valid_number(board,x,y)){
				board[x][y] = k;
				if(is_board_filled(board)){
					return true;
				} else {
					if(sudoku_filler(board)) return true;
				}
				// if(!finish) board[x][y] = 0;
				// else break;
			}
			board[x][y] = 0;
			break;
		}
	}
	

	return false;
}

bool valid_preboard(vector<vector<int>>& board,vector<int>& preset_pos){
	bool valid = true;
	for(int i = 0;i < preset_pos.size() && valid;++i){
		int x = preset_pos[i] / sz;
		int y = preset_pos[i] % sz;

		for(int j = 0;j < sz;++j){
			if(j != x && board[j][y] == board[x][y]){
				// cout << j * sz + y << " " << preset_pos[i] << "\n";
				valid = false;
				break;
			}
			if(j != y && board[x][j] == board[x][y]){
				// cout << x * sz + j << " " << preset_pos[i] << "\n";
				valid = false;
				break;
			}
		}

		if(!valid) break;

		int grid_x = x / N * N;
		int grid_y = y / N * N;

		for(int j = 0;j < N && valid;++j){
			for(int k = 0;k < N;++k){
				if((grid_x + j != x || grid_y + k != y) && board[grid_x + j][grid_y + k] == board[x][y]){
					// cout << (grid_x + j) * sz + (grid_y + k) << " " << preset_pos[i] << "\n";
					valid = false;
					break;
				}
			}
		}
	}
	if(!valid){
		for(int i = 0;i < preset_pos.size();++i){
			int x = preset_pos[i] / sz;
			int y = preset_pos[i] % sz;
			board[x][y] = 0;
		}
	}
	return valid;
}

vector<int> valid_number(vector<vector<int>>& board,int& x,int& y){
	vector<bool> candidates(sz + 1,true);

	for(int i = 0;i < sz;++i){
		if(i != x && board[i][y] != 0) candidates[board[i][y]] = false;
		if(i != y && board[x][i] != 0) candidates[board[x][i]] = false;
	}
	int grid_x = x / N * N;
	int grid_y = y / N * N;

	for(int i = 0;i < N;++i){
		for(int j = 0;j < N;++j){
			if((grid_x + i != x || grid_y + j != y) && board[grid_x + i][grid_y + j] != 0){
				candidates[board[grid_x + i][grid_y + j]] = false;
			}
		}
	}

	vector<int> final_cand;
	for(int i = 1;i < candidates.size();++i){
		if(candidates[i]) final_cand.push_back(i);
	}
	random_shuffle(final_cand.begin(),final_cand.end());

	return final_cand;
}

void remove_spaces(string& s){
	string tmp = "";
	for(int i = 0;i < s.length();++i){
		if((s[i] - 'a' >= 0 && s[i] - 'a' < 26) || (s[i] - 'A' >= 0 && s[i] - 'A' < 26)) tmp += s[i];
	}
	s = tmp;
}

void input_clear(){
	while(getchar() != '\n');
}