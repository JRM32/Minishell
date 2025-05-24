#!/bin/bash
GREEN="\033[0;32m"
RED="\033[0;31m"
RESET="\033[0m"
TMP_OUTPUT=".tmp_output_minishell"

# Para comandos simples - toma solo la última línea
clean_output_single() {
    tail -n +2 "$TMP_OUTPUT" |
    sed 's/\x1b\[[0-9;]*[mGKHF]//g' | # Eliminar códigos de color ANSI
    sed 's/^miniyo\$[[:space:]]*//' | # Eliminar el prompt al inicio de la línea
    sed '/^[[:space:]]*$/d' |
    tail -n 1
}

# Para comandos múltiples - preserva saltos de línea
clean_output_multi() {
    local raw_output=$(tail -n +2 "$TMP_OUTPUT" |
        sed 's/\x1b\[[0-9;]*[mGKHF]//g' | # Eliminar códigos de color ANSI
        sed 's/^miniyo\$[[:space:]]*//' | # Eliminar el prompt al inicio de cada línea
        sed '/^[[:space:]]*$/d' |
        sed 's/[[:space:]]*$//' |
        awk '!/^echo(\s|$)/ && !/^\$kk(\s|$)/' )

    # Preservar el salto de línea final si existía en la salida original
    if [[ $(</dev/fd/0) =~ $'\n'$ ]]; then
        echo "$raw_output"
    else
        echo -n "$raw_output"
    fi
}


test_echo() {
    local input="$1"
    local expected="$2"
    echo "$input" | ../minishell > "$TMP_OUTPUT" 2>/dev/null
    local cleaned_output
    cleaned_output=$(clean_output_single)
    if [ "$cleaned_output" = "$expected" ]; then
        echo -e "${GREEN}✔️  $input${RESET}"
    else
        echo -e "${RED}❌  $input${RESET}"
        echo "    Esperado: \"$(echo -e "$expected")\""
        echo "    Obtenido: \"$(echo -e "$cleaned_output")\""
    fi
}

# Nueva función para múltiples líneas que preserva saltos
test_multiline() {
    local input="$1"
    local expected="$2"
    echo -e "$input" | ../minishell > "$TMP_OUTPUT" 2>/dev/null
    local cleaned_output
    cleaned_output=$(clean_output_multi)
    if [ "$cleaned_output" = "$expected" ]; then
        echo -e "${GREEN}✔️  ${input//\\n/ | }${RESET}"
    else
        echo -e "${RED}❌  ${input//\\n/ | }${RESET}"
        echo "    Esperado: \"$(echo -e "$expected")\""
        echo "    Obtenido: \"$(echo -e "$cleaned_output")\""
        echo "    Raw output:"
        cat "$TMP_OUTPUT" | sed 's/^/        /'
    fi
}

# -------------------
# Pruebas individuales
# -------------------
test_echo "echo hola" "hola\n"
test_echo "echo -n hola" "hola"
test_echo "echo -nnnn -n -n hola" "hola"
test_echo "echo $a\"\"a" "a\n"
test_echo "echo $a\"\" a" " a\n"
test_echo 'echo $a" $USER"' " ${USER}\n"
test_echo 'echo $a"$USER"' "${USER}\n"
test_echo 'echo $USERa$HOME' "${HOME}\n"
test_echo 'echo "$  a"' "$  a\n"
test_echo 'echo "$?u "' "${?}u\n"
test_echo 'echo "$? us "' "${?} us\n"
test_echo 'echo $a' "\n"
test_echo 'echo $"msg"' "msg\n"
test_echo 'echo $ "msg"' "$ msg\n"
test_echo "echo $'msg'" "msg\n"

# -------------------
# Pruebas con export
# -------------------
test_multiline "export kk=-n\necho \$kk patata" "patata"
test_multiline "export kk=echo -n msg1\n\$kk msg2" "msg1 msg2"
test_multiline "export kk=echo -nnnnn -n -nnnann msg1\n\$kk msg2" "-nnnann msg1 msg2"
test_multiline "export kk=echo msg1\n\$kk -n msg2" "msg1 -n msg2"

# Limpieza
rm -f "$TMP_OUTPUT"
