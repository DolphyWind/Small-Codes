#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_LOOPS
#define MAX_LOOPS 10000
#endif

#ifndef N
#define N 30000
#endif

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		fprintf(stderr, "Error: No input file\n");
		return 1;
	}
	
	// Read file content into bfcode
	FILE *f = fopen(argv[1], "r");
	char *bfcode = 0;
	size_t codeLength = 0;
	if(f)
	{
		fseek(f, 0, SEEK_END);
		codeLength = ftell(f);
		fseek(f, 0, SEEK_SET);

		bfcode = malloc(codeLength * sizeof(char));
		if(bfcode)
		{
			fread(bfcode, 1, codeLength, f);
		}
		else
		{
			fprintf(stderr, "An error occured!\n");
			fclose(f);
			free(bfcode);
		}

		fclose(f);
	}
	else
	{
		fprintf(stderr, "Error: Unable to open file!\n");
		return 1;
	}
    
    
	size_t loopCount = 0;
	size_t currentCell = 0;
    
    // Loop stack
	size_t *loopPositions = calloc(MAX_LOOPS, sizeof(size_t));
	// Memory tape
	unsigned char *memory = calloc(N, sizeof(unsigned char));
    
    // Interpereter
	for(size_t i = 0; i < codeLength; i++)
	{
		char current = bfcode[i];
		if (current == '+')
			memory[currentCell] ++;
		else if (current == '-')
			memory[currentCell] --;
		else if(current == '>')
		{
			currentCell ++;
			if(currentCell >= N)
				currentCell = 0;
		}
		else if(current == '<')
		{
			currentCell --;
			if(currentCell < 0)
				currentCell = N - 1;
		}
		else if(current == '.')
			putchar(memory[currentCell]);
		else if(current == ',')
			memory[currentCell] = getchar();
		else if(current == '[')
		{
			if(!memory[currentCell])
			{
				while(bfcode[i] !=']' && i < codeLength)
					i++;
			}
			else
			{
				loopCount ++;
				loopPositions[loopCount - 1] = i;
			}
		}
		else if(current == ']')
		{
			if(!memory[currentCell])
				loopCount --;
			else
				i = loopPositions[loopCount - 1];
		}
	}

	free(loopPositions);
	free(memory);
	free(bfcode);
	return 0;
}
