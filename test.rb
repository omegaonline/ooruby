require 'OORuby'

test = Omega::CreateInstance('Test.Library')

$stdout.print("test is an instance of ", test.class, "\n")

if test.BoolNot1(true) != false then puts "BoolNot1" end

r = true
r = test.BoolNot2(true,r)
if r != false then puts "BoolNot2" end

r = true
r = test.BoolNot3(true,r)
if r != false then puts "BoolNot3" end

r = true
r = test.BoolNot4(r)
if r != false then puts "BoolNot4" end

puts "Press ENTER to finish"
gets
