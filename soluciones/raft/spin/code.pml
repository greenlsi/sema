ltl spec_code {
    [] (((input[0] != secret[0]) ||
         (input[1] != secret[1]) ||
         (input[2] != secret[2])) -> (current != 3))
}

ltl spec_light {
    [] (((state == 0) && buttonl) -> <> (state == 1)) &&
    [] ((state == 1) -> <> (state == 0))
}

#define timeout true


/* --------------- ALARM (simplified) ------------*/

bit code_ok;


active proctype alarm_fsm () {
    do
    :: code_ok -> code_ok
    od
}


/* --------------- CODE ------------------------*/

bit button;
bit started;

byte secret[3] = { 1, 2, 3 };
byte input[4];
int current;

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


/* --------------- LIGHT -----------------------*/

bit buttonl;
byte state;

active proctype lampara_fsm () {
    state = 0;
    do
    :: (state == 0) -> atomic {
        if
        :: buttonl -> state = 1; buttonl = 0
        fi
    }
    :: (state == 1)  -> atomic {
        if
        :: buttonl -> state = 0; buttonl = 0
        :: timeout -> state = 0
        fi
    }
    od
}


/* --------------- ENTORNO ---------------------*/

active proctype entorno () {
    do
    :: if
       :: button = 1
       :: buttonl = 1
       :: skip
       fi;
       printf ("CODE:  button = %d, input=%d%d%d [%d], code_ok = %d\n",
               button, input[0], input[1], input[2], current, code_ok)
       printf ("LIGHT: buttonl = %d, state = %d\n",
               buttonl, state)
    od
}
