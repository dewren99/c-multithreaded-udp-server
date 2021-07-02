UNEXPECTED ERRORS THAT SHOULD NOT EXIST THAT I COULD NOT FIX:
- there will be at most 2 definitely lost memory blocks when run on valgrind
- there is no support for slicing the message into multiple UDP datagrams if the message is too large, overflowing part is not sent

EXPECTED ERRORS DUE TO THE SCOPE OF THE ASSIGNMENT:
- under stress test (feeding 2mb dictionary to local and remote at the same time) the s-talk eventually crashes because List ADT is not thread safe
I've mentioned this to the Prof and I was told that I dont have to cover this case (June 29th Office Hours)