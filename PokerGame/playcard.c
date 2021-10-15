#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// global variable for a deck of cards
char deckOfCards[52][3] = {"D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DT", "DJ", "DQ", "DK", "DA",
                           "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CT", "CJ", "CQ", "CK", "CA",
                           "H2", "H3", "H4", "H5", "H6", "H7", "H8", "H9", "HT", "HJ", "HQ", "HK", "HA",
                           "S2", "S3", "S4", "S5", "S6", "S7", "S8", "S9", "ST", "SJ", "SQ", "SK", "SA"};
       
int patternFinder(char *hand) {
        int i;
        int pair=0, straight=0, flush=0, threeSame=0, fourSame=0, consecutive=0;
        int suitCount[13]={0}, rankCount[13]={0};
        for (i=0; i<strlen(hand); i+=3) {
                switch (hand[i]) {
                        case 'D': suitCount[0]++; break;
                        case 'C': suitCount[1]++; break;
                        case 'H': suitCount[2]++; break;
                        case 'S': suitCount[3]++; break;
                        default: printf("Something is wrong with your program\n");
                }
                switch (hand[i+1]) {
                        case '2': rankCount[0]++; break;
                        case '3': rankCount[1]++; break;
                        case '4': rankCount[2]++; break;
                        case '5': rankCount[3]++; break;
                        case '6': rankCount[4]++; break;
                        case '7': rankCount[5]++; break;
                        case '8': rankCount[6]++; break;
                        case '9': rankCount[7]++; break;
                        case 'T': rankCount[8]++; break;
                        case 'J': rankCount[9]++; break;
                        case 'Q': rankCount[10]++; break;
                        case 'K': rankCount[11]++; break;
                        case 'A': rankCount[12]++; break;
                        default: printf("Something is real wrong\n");
                }
        }

        for (i=0; i<4; i++) {
                if (suitCount[i] == 5) {
                        flush = 1;
                }
        }

        for (i=0; i<13; i++) {
                if (rankCount[i] >= 2) {
                        pair++;
                }
                if (rankCount[i] >= 3) {
                        threeSame = 1;
                }
                if (rankCount[i] >= 4) {
                        fourSame = 1;
                }
        }

        for (i=0; i<26; i++) {
                if (rankCount[i%13] == 1) {
                        consecutive++;
                }
                else {
                        consecutive = 0;
                }
                if (consecutive  == 5) {
                        straight = 1;
                }
        }
        
        if (flush && straight) return 0;
        if (fourSame) return 1;
        if (threeSame && pair == 2) return 2;
        if (flush) return 3;
        if (straight) return 4;
        if (threeSame) return 5;
        if (pair == 2) return 6;
        if (pair == 1) return 7;
        return 8;
}

void patternPrinter (int patternNum) {
        switch (patternNum) {
                case 0: printf("royal-flush"); break;
                case 1: printf("four-of-a-kind"); break;
                case 2: printf("full-house"); break;
                case 3: printf("flush"); break;
                case 4: printf("straight"); break;
                case 5: printf("three-of-a-kind"); break;
                case 6: printf("two-pairs"); break;
                case 7: printf("pair"); break;
                case 8: printf("nothing"); break;
        }
}

int cardExchangeStrat (char *hand) {
        // we need to find out all possible combination of exchanging the cards
        // if 1 out of 5 card exchange has the best probability, we do the exchange using that card
        // however, if none of the card is good for exchange, we just pass
        
        // get the pattern of the original hand first
        int originalPattern = patternFinder(hand);

        // now we test whether exchanging the card is good or not
       char exchangeHand[16], searchCardInHand[3], tempCard[3];
        int i,j,k;
        int conflict;
        int goodCombi=0, badCombi=0;
        int bestCardIndex=-1 ; // the card that if it is exchanged, the most good combination can be retireved
        char bestCardChar[3]; //for knowing whether this is the lowest card
        int bestCombi=0;
        // for each card in hand, it can be replaced by any card in the deck (except those repeated) 
        for (i=0; i<strlen(hand); i+=3) {
                strcpy(exchangeHand, hand);
                goodCombi = 0;
                badCombi = 0;
                // looping each card in the deck
                for (j=0; j<52; j++) {
                        // we choose only the card that is not in our hand
                        conflict = 0;
                        for (k=0; k<strlen(hand); k+=3) {
                                strncpy(searchCardInHand, hand+k, 2);
                                searchCardInHand[2] = 0;
                                if (strcmp(searchCardInHand, deckOfCards[j]) == 0) {
                                        conflict = 1;
                                }
                        }

                        if (!conflict) {
                                // replacing the card in hand with a random from deck
                                strncpy(exchangeHand+i, deckOfCards[j], 2);
                                // if the card does not diminish the strength of the deck, then it is counted as good
                                if (patternFinder(exchangeHand) <= originalPattern) {
                                        goodCombi++;
                                }
                                else {
                                        badCombi++;
                                }
                        }
                }
                strncpy(tempCard, hand+i, 2);
                tempCard[2] = 0;
                if (goodCombi > bestCombi || (bestCombi == goodCombi && isLowerCard(tempCard, bestCardChar))) {
                        if (goodCombi > badCombi) {
                                bestCombi = goodCombi;
                                bestCardIndex = i;
                                strcpy(bestCardChar, tempCard);
                        }
                }

        }
        return bestCardIndex;
}

int isLowerCard (char *tempCard, char*bestCardChar) {
        int temp, best, i;
        if (strlen(bestCardChar) == 0) {
                strcpy(bestCardChar, "D2");
        }
        char deckOfCardsNoSuit[13] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
        char deckOfCardsNoNum[4] = {'D', 'C', 'H', 'S'};
        for (i=0; i<13; i++) {
                if (tempCard[1] == deckOfCardsNoSuit[i]) {
                        temp = i;
                }
                if (bestCardChar[1] == deckOfCardsNoSuit[i]) {
                        best = i;
                }
        }
        if (temp == best) {
                for (i=0; i<4; i++) {
                        if (tempCard[0] == deckOfCardsNoNum[i]) {
                                temp = i;
                        }
                        if (bestCardChar[0] == deckOfCardsNoNum[i]) {
                                best = i;
                        }
                }
                if (temp < best) return 1;
                return 0;
        }
        if (temp < best) return 1;
        return 0;
}

int isSameTypeStronger (char *challengerHand, char *winnerHand) {
        // we first count the suit and number of the the hands
        int challengerNumCounter[13] = {0};
        int challengerSuitCounter[4] = {0};
        int winnerNumCounter[13] = {0};
        int winnerSuitCounter[4] = {0};
        int i;

        for (i=0; i<strlen(challengerHand); i+=3) {            
                switch (challengerHand[i]) {
            		case 'D': challengerSuitCounter[0]++; break;
                    case 'C': challengerSuitCounter[1]++; break;
                    case 'H': challengerSuitCounter[2]++; break;
                    case 'S': challengerSuitCounter[3]++; break;
                    default: printf("Something is wrong with your program\n");
                }
                switch (challengerHand[i+1]) {
	            	case '2': challengerNumCounter[0]++; break;
	                case '3': challengerNumCounter[1]++; break;
	                case '4': challengerNumCounter[2]++; break;
	                case '5': challengerNumCounter[3]++; break;
	                case '6': challengerNumCounter[4]++; break;
	                case '7': challengerNumCounter[5]++; break;
	                case '8': challengerNumCounter[6]++; break;
	                case '9': challengerNumCounter[7]++; break;
	                case 'T': challengerNumCounter[8]++; break;
	                case 'J': challengerNumCounter[9]++; break;
	                case 'Q': challengerNumCounter[10]++; break;
	                case 'K': challengerNumCounter[11]++; break;
	                case 'A': challengerNumCounter[12]++; break;
	                default: printf("Something is real wrong\n");
                }
				switch (winnerHand[i]) {
            		case 'D': winnerSuitCounter[0]++; break;
                    case 'C': winnerSuitCounter[1]++; break;
                    case 'H': winnerSuitCounter[2]++; break;
                    case 'S': winnerSuitCounter[3]++; break;
                    default: printf("Something is wrong with your program\n");
                }
                switch (winnerHand[i+1]) {
	            	case '2': winnerNumCounter[0]++; break;
	                case '3': winnerNumCounter[1]++; break;
	                case '4': winnerNumCounter[2]++; break;
	                case '5': winnerNumCounter[3]++; break;
	                case '6': winnerNumCounter[4]++; break;
	                case '7': winnerNumCounter[5]++; break;
	                case '8': winnerNumCounter[6]++; break;
	                case '9': winnerNumCounter[7]++; break;
	                case 'T': winnerNumCounter[8]++; break;
	                case 'J': winnerNumCounter[9]++; break;
	                case 'Q': winnerNumCounter[10]++; break;
	                case 'K': winnerNumCounter[11]++; break;
	                case 'A': winnerNumCounter[12]++; break;
	                default: printf("Something is real wrong\n");
                }            
        }
        // we do corresponding comparison for each type of same hand
        int challengerLargestNum, winnerLargestNum, challengerLargestSuit, winnerLargestSuit;
        switch (patternFinder(challengerHand)) {
                // royal-flush: check number first, and then suit
                case 0: 
                for (i=0; i<13; i++) {
                        if (challengerNumCounter[i] == 1) {
                                challengerLargestNum = i;
                        }  
                        if (winnerNumCounter[i] == 1) {
                                winnerLargestNum = i;
                        }

                }
                for (i=0; i<4; i++) {
                        if (challengerSuitCounter[i] == 5) {
                                challengerLargestSuit = i;
                        } 
                        if (winnerSuitCounter[i] == 5) {
                                winnerLargestSuit = i;
                        }
                }
                if (challengerLargestNum > winnerLargestNum) {
                        return 1;
                }
                else {
                        if (challengerLargestNum == winnerLargestNum && challengerLargestSuit > winnerLargestSuit) {
                                return 1;
                        }
                        return 0;
                }
                break;

                case 1: 
                // for four-of-a-kind, we just check the number
                for (i=0; i<13; i++) {
                        if (challengerNumCounter[i] == 4) {
                                challengerLargestNum = i;
                        }
                        if (winnerNumCounter[i] == 4) {
                                winnerLargestNum = i;
                        }
                }
                if (challengerLargestNum > winnerLargestNum) return 1;
                return 0;
                break;

                case 2:
                // for full-house, we check the number only
                for (i=0; i<13; i++) {
                        if (challengerNumCounter[i] == 3) {
                                challengerLargestNum = i;
                        }
                        if (winnerNumCounter[i] == 3) {
                                winnerLargestNum = i;
                        }
                }
                if (challengerLargestNum > winnerLargestNum) return 1;
                return 0;
                break;
                
                case 3:
                // for flush, we check the suit only
                for (i=0; i<13; i++) {
                        if (challengerSuitCounter[i] == 5) {
                                challengerLargestNum = i;
                        }  
                        if (winnerSuitCounter[i] == 5) {
                                winnerLargestNum = i;
                        }
                }
                if (challengerLargestNum > winnerLargestNum) return 1;
                return 0;
                break;

                case 4: 
                // for straight, we check the number only
                for (i=0; i<13; i++) {
                        if (challengerNumCounter[i] == 1) {
                                challengerLargestNum = i;
                        }  
                        if (winnerNumCounter[i] == 1) {
                                winnerLargestNum = i;
                        }

                }
                if (challengerLargestNum > winnerLargestNum) return 1;
                return 0;
                break;

                case 5:
                // for three-of-a-kind, we check the number only
                for (i=0; i<13; i++) {
                        if (challengerNumCounter[i] == 3) {
                                challengerLargestNum = i;
                        }
                if (winnerNumCounter[i] == 3) {
                                winnerLargestNum = i;
                        }
                }
                
                if (challengerLargestNum > winnerLargestNum) return 1;
                return 0;
                break;

                case 6:
                case 7:
                // for two-pairs, check the largest pair num
                for (i=0; i<13; i++) {
                        if (challengerNumCounter[i] == 2) {
                                challengerLargestNum = i;
                        }
                        if (winnerNumCounter[i] == 2) {
                                winnerLargestNum = i;
                        }
                }
                if (challengerLargestNum > winnerLargestNum) return 1;
                return 0;
                break;

                case 8:
                // for nothing, just find the largest card
                for (i=0; i<13; i++) {
                        if (challengerNumCounter[i] == 1) {
                                challengerLargestNum =i ;
                        }
                        if (winnerNumCounter[i] == 1) {
                                winnerLargestNum = i;
                        }
                }
                for (i=0; i<5; i++) {
                        if (challengerSuitCounter[i] == 1) {
                                challengerLargestSuit = i;
                        }
                        if (winnerSuitCounter[i] == 1) {
                                winnerLargestSuit = i;
                        }
                }
                if (challengerLargestNum > winnerLargestNum || (challengerLargestNum == winnerLargestNum && challengerLargestSuit > winnerLargestSuit)) {
                        return 1;
                }
                else return 0;
                break;
        }
}
        
                                


int main (int argc, char *argv[]) {
        int pid, i, j, childNum, roundNum;
        char rawCardString[10000] = {0};
        int parentToChildPipe[10][2];
        int childToParentPipe[10][2];
        int childCount = atoi(argv[1]);
        char childResponse[childCount][5];
        char childTempDeck[childCount][16];
        char childCard[16];
        char tempString[10];

        // read the file via stdin
        int charRead = read(STDIN_FILENO, rawCardString, 10000);
        // create two pipes between parent and child
        
        for (i=0; i<childCount; i++) {
                childNum = i;
                if (pipe(parentToChildPipe[i]) < 0) {
                        printf("Pipe broken. Please find a plumber\n");
                }
                if (pipe(childToParentPipe[i]) < 0) {
                        printf("Pipe broken. Please find a plumber\n");
                }
                pid = fork();
                if (pid == 0) break; // break the loop when it is child
                if (pid < 0) printf("Fork failed. Might consider using chopsticks\n");
        }
        

        // if it is child
        if (pid == 0) {
                // child should not write in parentToChildPipe
                // close the writing side of it 
                close(parentToChildPipe[childNum][1]);
                // child should not read in childToChildPipe
                // close the reading side of it 
                close(childToParentPipe[childNum][0]);
                // collect the card from the parent
                int cardCollected = 0;
                int bestCardIndex = -1;
                char response[5] = {0};
                char collected[5] = {0};
                charRead = read(parentToChildPipe[childNum][0], childCard, 16);
                childCard[charRead] = 0;
                printf("Child %d pid %d: hand <%s>\n", childNum+1, getpid(), childCard);
                sleep(1);
                char originalChildCard[15];
                strcpy(originalChildCard, childCard);

                // respond to the parent, which is usually what children dont do these days
                for (i=0; i<childCount; i++) {
                        sleep(1);
                        printf("Child %d, pid %d: type is ", childNum+1, getpid());
                        patternPrinter(patternFinder(childCard));
                        bestCardIndex = cardExchangeStrat(childCard);
                        if (bestCardIndex == -1) {
                                printf(", no exchange\n");
                                // write into the pipe to respond to the parentsi
                                strcpy(response, "pass");
                                write(childToParentPipe[childNum][1], response, 5);
                        }
                        else {
                                printf(", exchange card ");
                                strncpy(response, childCard+bestCardIndex, 2);
                                response[2] = 0;
                                printf("%s\n", response);
                                write(childToParentPipe[childNum][1], response, 5);
                        }
                        
                        sleep(1);
                        // collect the new cards, if we are not pass
                        if (bestCardIndex != -1) {
                                // swap the card
                                read(parentToChildPipe[childNum][0], collected, 5);
                                strncpy(childCard+bestCardIndex, collected, 2);
                                //printf("Child %d collected %s\n", childNum, collected);
                                printf("Child %d, pid %d: new card %s, new hand <%s>\n", childNum+1, getpid(),  collected, childCard);
                        }
                        sleep(1);
                }


                if (patternFinder(childCard) < patternFinder(originalChildCard)) {
                        printf("Child %d, pid %d: improve from ", childNum+1, getpid());
                        patternPrinter(patternFinder(originalChildCard));
                        printf(" to ");
                        patternPrinter(patternFinder(childCard));
                        printf("\n");
                }
                else {
                        printf("Child %d, pid %d: no improvement for ", childNum+1, getpid());
                        patternPrinter(patternFinder(originalChildCard));
                        printf("\n");
                }
                sleep(1);
                // send the hand to the parent
                write(childToParentPipe[childNum][1], childCard, 16);
                exit(0);
        }
        
        // if it is parent
        else {
                for (i=0; i<childCount; i++) {
                        // parent should not read in parentToChildPipe
                        // close the reading side of it 
                        close(parentToChildPipe[i][0]);
                        // parent should not write in childToParentPipe
                        // close the writing side of it
                        close(childToParentPipe[i][1]);
                }
                
                printf("Parent, pid %d: there are %d children\n", getpid(), childCount);
                
                // parents deal the cards in a round-robin way
                // we first form a hand for each child
                int cardPointer = 0;
                for (i=0; i<5; i++) {
                        for (j=0; j<childCount; j++) {
                                if (i == 0) childTempDeck[j][0] = 0; // if it is the first card, initialize the array beforehand
                                strncpy(tempString, rawCardString+cardPointer, 2); // e.g. extract C3 from ... C3 C4 C5 ...
                                tempString[2] = i == 4 ? 0 : ' ';
                                strcat(childTempDeck[j], tempString);           // e.g. add C3 to the tempdeck: C4 C5 --> C4 C5 C3 
                                cardPointer += 3;
                        }
                }
                // we then pass the hand to the children through the pipe
                for (i=0; i<childCount; i++) {
                        write(parentToChildPipe[i][1], childTempDeck[i], 15);
                }

                // parents then ask the children the response
                for (roundNum=0; roundNum<childCount; roundNum++) {
                       printf("Parent, pid %d: round %d asking\n", getpid(), roundNum+1);
                       // read each child's response
                        for (i=0; i<childCount; i++) {
                                read(childToParentPipe[i][0], childResponse[i], 5);
                        }
                        sleep(1);
                        // swap the cards according to the response
                        printf("Parent, pid %d: round %d sending\n", getpid(), roundNum+1);
                        i = 0;
                        while (i < childCount)  {
                                if (strcmp(childResponse[i], "pass") != 0) {
                                        //printf("Child %d is not a pass\n", i+1);
                                        j=i+1;
                                        while (strcmp(childResponse[j%childCount], "pass") == 0) {
                                                                                             j++;
                                        }
                                        //printf("Child %d give child %d\n", i+1, j%childCount+1);
                                        write(parentToChildPipe[j%childCount][1], childResponse[i], 5);
                                        i = j;
                                }
                                else {
                                        i++;
                                }
                        }
                        sleep(2);
                }
                sleep(1);
                // read the children's hand
                char collectChildCard[childCount][16];
                int winnerPattern = 9;
                int winnerNum = -1;
                char winnerHand[16];
                for (i=0; i<childCount; i++) {
                        read(childToParentPipe[i][0], collectChildCard[i], 16);
                        printf("Parent, pid %d: child %d hand <%s> is ", getpid(), i+1, collectChildCard[i]);
                        patternPrinter(patternFinder(collectChildCard[i]));
                        if ((patternFinder(collectChildCard[i]) < winnerPattern)) {                                
                                winnerPattern = patternFinder(collectChildCard[i]);
                                winnerNum = i;
                                strcpy(winnerHand, collectChildCard[i]);
                        }
                        if ((patternFinder(collectChildCard[i]) == winnerPattern) && (isSameTypeStronger(collectChildCard[i], winnerHand))) {
                                winnerPattern = patternFinder(collectChildCard[i]);
                                winnerNum = i;
                                strcpy(winnerHand, collectChildCard[i]);
                        }
                        printf("\n");
                }
                printf("Parent, pid %d: child %d is the winner\n", getpid(), winnerNum+1);

                        
                
                wait(NULL);
        }

        return 0;
}


