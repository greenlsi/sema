ltl spec {
    [] (((input[0] != secret[0]) ||
         (input[1] != secret[1]) ||
         (input[2] != secret[2])) -> (current != 3))
}

#define timeout true

bit button;
bit code_ok;
bit started;

byte secret[3] = { 1, 2, 3 };
byte input[4];
int current;

active proctype alarm_fsm () {
    do
    :: code_ok -> code_ok = 0
    od
}

active proctype code_fsm () {
    code_ok = 0;
    current = 0;
    do
    :: true -> atomic {
        if
        :: (current >= 3) ->
            code_ok = 1; current = 0

        :: (timeout && started && (current < 3)
            && (input[current] == secret[current])) ->
            current++; input[current] = 0

        :: (timeout && started && (current < 3)
            && (input[current] != secret[current])) ->
            current = 0; input[current] = 0

        :: (button && (current < 3)) ->
            input[current] = (input[current] + 1) % 10;
            button = 0; started = 1
        fi
    }
    od
}

active proctype entorno () {
    do
    :: if
       :: button = 1
       :: (!button) -> skip
       fi ;
       printf ("button = %d, input=%d%d%d [%d], code_ok = %d\n",
               button, input[0], input[1], input[2], current, code_ok)
    od
}
