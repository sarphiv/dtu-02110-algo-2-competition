#!/bin/bash

for test_input in tests/*.in; do
    if [[ $test_input =~ (.*)\.in ]]; then
        OUTPUT=$(./main $test_input)
        ANSWER=$(cat ${BASH_REMATCH[1]}.ans)
        if [[ $OUTPUT == $ANSWER ]]; then
            echo "✅ ${BASH_REMATCH[1]}"
        else
            echo -e "❌ ${BASH_REMATCH[1]}\t OUT:$OUTPUT\t != ANS:$ANSWER"
        fi
    else
        echo "Bug in test script for path: $test_input"
    fi
done


exit 0