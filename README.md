The TextPrinter class can be used to pass a string of text to and get an SDL surface back from
with the text printed.
Several things like font and font-size have to be specified in the constructor and cannot be changed
anymore after creation. (You'd have to create another one for another font or another font-size).
Other things can be specified on the fly using meta-text within the strings passed to the TextPrinter.
To incidate the start or end of a meta-text field use the following symbols: /\< for start, /\> for end.
Example: "Hey there /\<FPC=10/\>.../\<FPC=4/\> Uhm/\<FPC=10/\>.../\<FPC=4/\> I wanna say Johnny?"
Furthermore if you want to print a backslash (\) character you must always escape it. This is how the
TextPrinter can distinguish between meta- and normal text. So if for some reason you actually want to
print /\<FPC=10/\> literally. You'd have to write it like /\\<FPC=10/\\>
Not escaping a backslash anywhere other than in the meta-text open and close tags will cause an error.
The syntax for the meta-text field will be listed here. (Wrong syntax use will of course also result in
an error):
#FPC=<unsigned integer> 
	Frames Per Character specifies the rate at which each character is printed.
	Example: if FPC is 3, assuming you call TextPrinter.next() every frame, a new character will be
	added every three frames to the SDL surface pointer that it returns. If you set it to zero it will
	print everything at once. leaving its value blank or entering anything other than digits will result
	in an error.
#COLOR=\d\d\d,\d\d\d,\d\d\d
	This sets the current colour of the text to be rendered (rgb).
	Example: /\<COLOR=255,128,0/\>
	Don't add spaces anywhere.
	PS. I am a fan of British spelling so COLOUR will also be accepted.
#NONE
	This sets the render effect to NONE.
#WAVE
	This sets the render effect to WAVE.
#BOUNCE
	This sets the render effect to BOUNCE
#SHAKE
	This sets the render effect to SHAKE.
#SPIN
	This sets the render effect to SPIN.