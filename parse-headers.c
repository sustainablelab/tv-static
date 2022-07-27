/* *************DOC***************
 * $ ./parse-headers.exe        -- list all headers
 * $ ./parse-headers.exe M      -- only list headers for my libs
 * *******************************/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum _state__def { STR, SEP } state;        // State is STRING or SEPARATOR
state update_state(const char c)
{
    state st = ((c == ' ') || (c == '\\') || (c == '\n')) ? SEP : STR;
    return st;
}
#define STR_OK (  (cnt>2) && (ignore==false)  )

int main(int argc, char *argv[])
{
    // Check for cmdline flag M
    bool just_tag_my_libs = false;
    {
        if(argc>1) if(argv[1][0]=='M') if(argv[1][1]=='\0') just_tag_my_libs = true;
    }

    FILE *f = fopen("headers-M.txt", "r");                      // Read output from gcc -M
    FILE *o = fopen("headers.txt", "w");                        // Write headers for ctags

    { // Parse headers.txt for header paths
        state st = SEP;                             // Initial state: outside of a STRING
        int cnt = 0;                                // Initial STRING count: 0
        int c;                                      // Parse file one character at a time
        bool ignore = false;                        // State: ignore this string or not
        while( (c=fgetc(f)) != EOF )
        {
            switch(st)
            {
                case SEP:                           // Outside a STRING
                    st = update_state(c);           // Update state
                    switch(st)                      // Do action based on new state
                    {
                        case STR:                   // Start of a new STRING
                            if(just_tag_my_libs)    // Ignore paths that start with C
                            {
                                if(c=='C') ignore = true;
                                else ignore = false;
                            }
                            cnt++;                  // Increment STRING counter
                            if(STR_OK) putc(c, o);  // Print all strings after first two: "blah.o: blah.c"
                            break;
                        case SEP:                   // Still outside a STRING
                            break;                  // Do nothing
                    }
                    break;
                case STR:                           // Inside a STRING
                    st = update_state(c);           // Update state
                    switch(st)                      // Do action based on new state
                    {
                        case STR:                   // Still inside a STRING
                            if(STR_OK) putc(c, o);  // Still printing this STRING
                            break;
                        case SEP:                   // STRING is finished
                            if(STR_OK) putc('\n', o); // Add a newline after all the strings I keep
                            break;
                        default: printf("Unexpected state: %d", st); fclose(f); return EXIT_FAILURE;
                    }
                    break;
                default: printf("Unexpected state: %d", st); fclose(f); return EXIT_FAILURE;
            }
        }
    }
    fclose(o);
    fclose(f);
    return EXIT_SUCCESS;
}
