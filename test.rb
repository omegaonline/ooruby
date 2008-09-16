require 'OORuby'

test = Omega::CreateInstance('Test.Library')

$stdout.print("test is an instance of ", test.class, "\n")

if test.BoolNot1(false) != true then puts "failed1" end
if test.BoolNot1(true) != false then puts "failed2" end

r = true
test.BoolNot2(true,r)
puts r
if r != false then puts "Failed3" end

puts "Press ENTER to finish"
gets
