// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_DEPTH 10
char history[HISTORY_DEPTH][COMMAND_LENGTH];
int history_count = 0;


/**
 * Command Input and Processing
 */

 void addCommandToHistory(char *buff){
 	history_count++;

 	int temp=history_count % 10;
 	for(int i=0; i < strlen(buff); i++){
 		history[temp][i] = buff[i];
 	}
 }

void sigint_handler() {
	write(STDOUT_FILENO, "\nexit\texit the shell program.\npwd\tdisplay the current working directory.\ncd\tchanging the current working directory.\n", strlen("\nexit\texit the shell program.\npwd\tdisplay the current working directory.\ncd\tchanging the current working directory.\n"));
	char dir[80];//directory
	getcwd(dir, sizeof(dir));
	write(STDOUT_FILENO, dir, strlen(dir));
	write(STDOUT_FILENO, "$ ", strlen("$ "));
}


/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */
int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);
	for (int i = 0; i < num_chars; i++) {
		switch (buff[i]) {
		// Handle token delimiters (ends):
		case ' ':
		case '\t':
		case '\n':
			buff[i] = '\0';
			in_token = false;
			break;

		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff[i];
				token_count++;
				in_token = true;
			}
		}
	}
	tokens[token_count] = NULL;
	return token_count;
}

/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *tokens[], _Bool *in_background, int* numOfTokens)
{
	*in_background = false;

	// Read input
	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);

	if ((length < 0) && (errno !=EINTR)) {
		perror("Unable to read command from keyboard. Terminating.\n");
		exit(-1);
	}

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}

	// load command from buff to history
	addCommandToHistory(buff);

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		*numOfTokens = 0;///////////////////
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
		*numOfTokens = token_count-1;///////
	}
	*numOfTokens = token_count;///////////
}





void printHistory(){
	write(STDOUT_FILENO, "---\n", strlen("---\n"));//////
	for(int i = history_count; (i > 0) && (i > history_count-10); i--){
		char countStr[10];
		sprintf(countStr,"%d",i);//////////////////
		int indexInHist = i % 10;
		write(STDOUT_FILENO, countStr, strlen(countStr));
		write(STDOUT_FILENO, "\t", strlen("\t"));
		write(STDOUT_FILENO, history[indexInHist], strlen(history[indexInHist]));
		write(STDOUT_FILENO, "\n", strlen("\n"));
	}
}

_Bool HistToBuff(int index, char *buff, char* tokens[]){
	if((tokens[0][1] == '!') && (tokens[0][2] == '\0')){
		if(history_count == 1){// if !! is the first commend
			history_count--;
			write(STDOUT_FILENO, "no previous command\n", strlen("no previous command\n"));
			return false;
		}
		for(int i=0; i<strlen(history[history_count-1]); i++){
			buff[i] = history[history_count-1][i];
		}
		buff[strlen(history[history_count-1])] = '\0';
		write(STDOUT_FILENO, buff, strlen(buff));
		history_count--;
		return true;
	}
	else if((index > 0) && (index > (history_count-10)) && (index <= history_count)){
		for(int i=0; i<strlen(history[index%10]); i++){
			buff[i] = history[index%10][i];
		}
		buff[strlen(history[index%10])] = '\0';
		write(STDOUT_FILENO, buff, strlen(buff));
		history_count--;
		return true;
	}
	else{
		write(STDOUT_FILENO, "the index is out of history range\n", strlen("the index is out of history range\n"));
		buff[0] = '\0';
		return false;
	}
}

void checkIfRedo(char* buff, char *tokens[], _Bool *in_background, int* numOfTokens){
	if(*numOfTokens == 1){
		if(tokens[0][0] == '!'){
			char str[COMMAND_LENGTH];
			for(int i=1; i<strlen(tokens[0]); i++){
				str[i-1] = tokens[0][i];
			}
			int target = atoi(str);
			_Bool isRedo = HistToBuff(target,buff,tokens);
			if (isRedo){
				buff[strlen(buff)] = '\0';
				if (buff[strlen(buff) - 1] == '\n') {
					buff[strlen(buff) - 1] = '\0';
				}

				// load command from buff to history
				addCommandToHistory(buff);

				// Tokenize (saving original command string)
				int token_count = tokenize_command(buff, tokens);
				if (token_count == 0) {
					*numOfTokens = 0;///////////////////
					return;
				}

				// Extract if running in background:
				if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
					*in_background = true;
					tokens[token_count - 1] = 0;
					*numOfTokens = token_count-1;///////
				}
				*numOfTokens = token_count;///////////
			}
		}
	}

}




/**
 * Main and Execute Commands
 */
int main(int argc, char* argv[])
{
	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];
	char previousDir[COMMAND_LENGTH];
	while (true) {

		// Get command
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().
		char dir[80];//directory
    getcwd(dir, sizeof(dir));
		write(STDOUT_FILENO, dir, strlen(dir));
		write(STDOUT_FILENO, "$ ", strlen("$ "));
		_Bool in_background = false;
		int numOfTokens = -1;
		read_command(input_buffer, tokens, &in_background, &numOfTokens);

		// signals
		struct sigaction s;
    s.sa_handler = sigint_handler;
    sigemptyset(&s.sa_mask);
    s.sa_flags = SA_RESTART;
    sigaction(SIGINT, &s, NULL);

		// DEBUG: Dump out arguments:///////////////////////////////////////////////////////////// Not necessary!!!!!
		// for (int i = 0; tokens[i] != NULL; i++) {
		// 	write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
		// 	write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
		// 	write(STDOUT_FILENO, "\n", strlen("\n"));
		// }
		// if (in_background) {
		// 	write(STDOUT_FILENO, "Run in background.", strlen("Run in background."));
		// }
		checkIfRedo(input_buffer, tokens, &in_background, &numOfTokens);
		/**
		 * Steps For Basic Shell:
		 * 1. Fork a child process
		 * 2. Child process invokes execvp() using results in token array.
		 * 3. If in_background is false, parent waits for
		 *    child to finish. Otherwise, parent loops back to
		 *    read_command() again immediately.
		 */


		 	pid_t var_pid;
			var_pid = fork();
			if (var_pid < 0) {
					fprintf (stderr, "fork Failed");
					exit(-1);
			}
			else if (var_pid == 0) { /* child process */
					if(numOfTokens>0 && (strcmp(tokens[0],"pwd") == 0)){
						break;
					}
					execvp(tokens[0], tokens);
					break;
			}
			else { /* parent process */
					/* parent will wait for child to complete */
					if(!in_background){
						waitpid(0,NULL,0);
					}
					//----------internal command----------//
					if(numOfTokens>0){
						if(strcmp(tokens[0],"exit") == 0){ //--- exit ---
							if(numOfTokens == 1){
									exit(0);
							}
							else{
								write(STDOUT_FILENO, "command not executed", strlen("command not executed"));
								write(STDOUT_FILENO, "\n", strlen("\n"));
							}
						}
						else if(strcmp(tokens[0],"pwd") == 0){ //--- pwd ---
								if(numOfTokens == 1){
									getcwd(dir, sizeof(dir));
									write(STDOUT_FILENO, dir, strlen(dir));
									write(STDOUT_FILENO, "\n", strlen("\n"));
								}
								else{
									write(STDOUT_FILENO, "command not executed", strlen("command not executed"));
									write(STDOUT_FILENO, "\n", strlen("\n"));
								}
						}
						else if(strcmp(tokens[0],"cd") == 0){ //--- cd ---
								char tempDir[COMMAND_LENGTH];
								getcwd(tempDir, sizeof(tempDir));
								if(numOfTokens == 1){
									int check=chdir("/home/");
									if(check == -1){
										write(STDOUT_FILENO, "change directory failed", strlen("change directory failed"));
										write(STDOUT_FILENO, "\n", strlen("\n"));
									}
									else{
										strcpy(previousDir,tempDir);
									}
								}
								else if(numOfTokens == 2){
									int check;
									if((tokens[1][0] == '~') && (tokens[1][1] == '/')){
										char newDirStr[COMMAND_LENGTH];
										newDirStr[0] = '/';
										newDirStr[1] = 'h';
										newDirStr[2] = 'o';
										newDirStr[3] = 'm';
										newDirStr[4] = 'e';
										for(int i=1; i<strlen(tokens[1]); i++){
											newDirStr[i+4] = tokens[1][i];
										}
										newDirStr[strlen(tokens[1]) + 5] = '\0';
										check=chdir(newDirStr);
									}
									else if((tokens[1][0] == '~') && (tokens[1][1] == '\0')){
										check=chdir("/home/");
									}
									else if((tokens[1][0] == '-') && (tokens[1][1] == '\0')){
										check=chdir(previousDir);
									}
									else{
										check=chdir(tokens[1]);
									}
									if(check == -1){
										write(STDOUT_FILENO, "change directory failed", strlen("change directory failed"));
										write(STDOUT_FILENO, "\n", strlen("\n"));
									}
									else{
										strcpy(previousDir,tempDir);
									}
								}
								else{
									write(STDOUT_FILENO, "command not executed", strlen("command not executed"));
									write(STDOUT_FILENO, "\n", strlen("\n"));
								}
						}
						if(strcmp(tokens[0],"help") == 0){ //--- help ---
							if(numOfTokens == 1){
									write(STDOUT_FILENO, "exit\texit the shell program.\npwd\tdisplay the current working directory.\ncd\tchanging the current working directory.\n", strlen("exit\texit the shell program.\npwd\tdisplay the current working directory.\ncd\tchanging the current working directory.\n"));
							}
							else if(numOfTokens == 2){
									if(strcmp(tokens[1],"exit") == 0){
										write(STDOUT_FILENO, "\'", strlen("\'"));
										write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
										write(STDOUT_FILENO, "\' is a builtin command for exiting the shell program.\n", strlen("\' is a builtin command for exiting the shell program.\n"));
									}
									else if(strcmp(tokens[1],"pwd") == 0){
										write(STDOUT_FILENO, "\'", strlen("\'"));
										write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
										write(STDOUT_FILENO, "\' is a builtin command for displaying the current working directory.\n", strlen("\' is a builtin command for displaying the current working directory.\n"));
									}
									else if(strcmp(tokens[1],"cd") == 0){
										write(STDOUT_FILENO, "\'", strlen("\'"));
										write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
										write(STDOUT_FILENO, "\' is a builtin command for changing the current working directory.\n", strlen("\' is a builtin command for changing the current working directory.\n"));
									}
									else{
										write(STDOUT_FILENO, "\'", strlen("\'"));
										write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
										write(STDOUT_FILENO, "\' is an external command or application.\n", strlen("\' is an external command or application.\n"));
									}
							}
							else{
								write(STDOUT_FILENO, "command not executed", strlen("command not executed"));
								write(STDOUT_FILENO, "\n", strlen("\n"));
							}
						}
						else if(strcmp(tokens[0],"history") == 0){ //--- history ---
							if(numOfTokens == 1){
									printHistory();
							}
							else{
								write(STDOUT_FILENO, "command not executed", strlen("command not executed"));
								write(STDOUT_FILENO, "\n", strlen("\n"));
							}
						}
					}

			}
			while(waitpid(-1, NULL, WNOHANG) > 0);//clean zombie processes////////////////////////////////////////////////////
	}
	return 0;
}
