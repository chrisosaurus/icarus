" Vim syntax file
" Language:             Icarus
" Maintainer:           Chris Hall
" Last Change:          2017 June 3
" Original Author:      Chris Hall

" syntax highlighting for the icarus programming languages
" https://github.com/mkfifo/icarus
"
" place as icarus.vim under ~/.vim/syntax/
" full path ~/.vim/syntax/icarus.vim
" add this to your ~/.vimrc
"    au BufRead,BufNewFile *.ic set filetype=icarus

if exists("b:current_syntax")
    finish
endif


"syntax match icarusOperator "{" "}" "[" "]" "(" ")"
"syntax match icarusOperator "+" "-" "/" "*"
"syntax match icarusOperator "<" ">" "=" "!" "==" "!""="
syntax match icarusOperator '&'

syntax region icarusString start='"' end='"'
syntax region icarusString start="'" end="'"

syntax keyword icarusOperator and or not

syntax keyword icarusKeyword begin if then else for while end
syntax keyword icarusKeyword let const
syntax keyword icarusKeyword in
syntax keyword icarusKeyword builtin
syntax keyword icarusKeyword fn func type enum union
syntax keyword icarusKeyword return
syntax keyword icarusKeyword import
syntax keyword icarusKeyword match case

syntax keyword icarusBoolean True False

" a type always begins with an uppercase letter
" and can then be made up of any combination of
"   a-z
"   A-Z
"   0-9
"   _
"   -
syntax match icarusType "\<\u[-_0-9a-zA-Z]*\>"

" a symbol always begins with a lowercase letter or an underscore
" and can then be made up on any combination of
"   a-z
"   A-z
"   0-9
"   _
"   -
" syntax match icarusSymbol '\<[_a-z][-_0-9a-zA-Z]*\>'

" decimals
syntax match icarusNumber '\d\+[su]'
syntax match icarusNumber '[-+]\d\+[su]'

" floating point
syntax match icarusFloat '\d\+\.\d\+f'
syntax match icarusFloat '[-+]\d\+\.\d\+f'


syntax match icarusComment '#.*$'


highlight link icarusString String
highlight link icarusKeyword Keyword
highlight link icarusNumber Number
highlight link icarusFloat Float
highlight link icarusBoolean Boolean

highlight link icarusComment Comment

highlight link icarusSymbol Identifier
highlight link icarusType Type

highlight link icarusOperator Operator


let b:current_syntax = "icarus"
