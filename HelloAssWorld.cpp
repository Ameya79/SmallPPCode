#!/bin/bash

BUNCHS="114 11122 112111 11311 1213 15 21112 11311 1123 2121 12112 21211"
MKey="V922/G/,2:"

export RotString=""
function initRotString() {
    local _i _char
    RotString=""
    for _i in {1..94} ;do
        printf -v _char "\\%03o" $((_i+32))
        printf -v RotString "%s%b" "$RotString" $_char
    done
}
 

function rotIO() {
    local _line _i _idx _key _cidx _ckey _o _cchar _kcnt=0
    while read -r _line ;do
        _o=""
        for (( _i=0 ; _i < ${#_line} ; _i++)) ;do
            ((_kcnt++ ))
            _cchar="${_line:_i:1}"
            [ "${_cchar//\(}" ] || _cchar="\("
            [ "${_cchar//\*}" ] || _cchar="\*"
            [ "${_cchar//\?}" ] || _cchar="\?"
            [ "${_cchar//\[}" ] || _cchar="\["
            [ "${_cchar//\\}" ] || _cchar='\\'
            if [ "${RotString//${_cchar}*}" == "$RotString" ] ;then
                _o+="${_line:_i:1}"
            else
                _kchar="${1:_kcnt%${#1}:1}"
                [ "${_kchar//\(}" ] || _kchar="\("
                [ "${_kchar//\*}" ] || _kchar="\*"
                [ "${_kchar//\?}" ] || _kchar="\?"
                [ "${_kchar//\[}" ] || _kchar="\["
                [ "${_kchar//\\}" ] || _kchar='\\'
                _key="${RotString//${_kchar}*}"
                _ckey=${#_key}
                _idx="${RotString//${_cchar}*}"
                _cidx=$(((1+_ckey+${#_idx})%94))
                _o+=${RotString:_cidx:1}
            fi; done
        if [ "$_o" ] ; then
            echo "$_o"
    fi ; done ; }
 

function rle() {
    local _out="" _c=1 _l _a=$1
    while [ "${_a}" ] ; do
        printf -v _l "%${_a:0:1}s" ""
        _out+="${_l// /$_c}"
        _a=${_a:1} _c=$((1-_c))
        done
    printf ${2+-v} $2 "%s" $_out
}
function binToChar() {
    local _i _func="local _c;printf -v _c \"\\%o\" \$(("
    for _i in {0..7} ;do
        _func+="(\${1:$_i:1}<<$((7-_i)))+"
        done
    _func="${_func%+}));printf \${2+-v} \$2 \"%b\" \$_c;"

    eval "function ${FUNCNAME}() { $_func }"
    $FUNCNAME $@
}

initRotString
 

for bunch in "${BUNCHS[@]}" ; do
    out=""
    bunchArray=($bunch)
    for ((k=0;k<${#bunchArray[@]};k++)) ; do
        enum=1
        if [ "${bunchArray[$k]:0:1}" == "-" ];then
            enum=${bunchArray[$k]:1}
            ((k++))
        fi
        ltr=${bunchArray[$k]}
        rle $ltr binltr
        printf -v bin8ltr "%08d" $binltr
        binToChar $bin8ltr outltr
        printf -v mult "%${enum}s" ""
        out+="${mult// /$outltr}"
    done
    rotIO "$MKey" <<< "$out"
done
