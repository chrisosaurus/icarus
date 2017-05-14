#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;
use IPC::Open3;

my $path = "./icarus";

die "Could not find '$path'\n" unless -e $path;

my $cases = [
  {
    input => '
      pushuint 176
      pushuint 7
      pushuint 27
      exit
      ',
    expected => '
      pancake value_stack after execution finished:
      ----------------
      uint: 27
      uint: 7
      uint: 176
      ----------------
      '
  },
  {
    input => '
      label dummy
      pushuint 4
      pushuint 3
      pushuint 6
      pushuint 7
      call_builtin plus(Uint,Uint) 2
      call_builtin plus(Uint,Uint) 2
      exit
      ',
    expected => '
      pancake value_stack after execution finished:
      ----------------
      uint: 16
      uint: 4
      ----------------
      '
  },
  {
    input => '
      pushuint 17
      call_builtin println(Uint) 1
      exit
      ',
    expected => '
      17
      pancake value_stack after execution finished:
      ----------------
      ----------------
      '
  },
  {
    input => '
      pushint 3
      pushint 5
      call_builtin minus(Sint,Sint) 2
      exit
      ',
    expected => '
      pancake value_stack after execution finished:
      ----------------
      sint: -2
      ----------------
      '
  },
  {
    input => '
      pushuint 3
      pushuint 5
      call_builtin minus(Uint,Uint) 2
      exit
      ',
    expected => '
      pancake value_stack after execution finished:
      ----------------
      uint: 0
      ----------------
      '
  },
  {
    input => '
      pushuint 3
      save
      exit
      ',
    expected =>'
      pancake value_stack after execution finished:
      ----------------
      ----------------
      ',
  },
  {
    input => '
      pushuint 3
      pushuint 5
      exit
      ',
    expected =>'
      pancake value_stack after execution finished:
      ----------------
      uint: 5
      uint: 3
      ----------------
      ',
  },
  {
    input => '
      pushuint 3
      save
      pushuint 5
      restore
      exit
      ',
    expected =>'
      pancake value_stack after execution finished:
      ----------------
      uint: 3
      uint: 5
      ----------------
      ',
  },
  # testing clean_stack correctly cleans up all locals and args
  {
    input => '
        label entry
        pushstr "Not an arg"
        pushuint 1
        pushuint 2
        pushuint 3
        call baz() 3
        exit
        label baz()
        pushuint 4
        pushuint 5
        clean_stack
        return_void
    ',
    expected => '
      pancake value_stack after execution finished:
      ----------------
      string: Not an arg
      ----------------
    ',
  },
  {
    input => '
      label entry
      call main() 0
      exit
      label foo()
      pushuint 5
      save
      clean_stack
      restore
      return_value
      label main()
      call foo() 0
      call_builtin println(Uint) 1
      clean_stack
      return_void
      ',
    expected => '
      5
      pancake value_stack after execution finished:
      ----------------
      ----------------
      ',
  },

  {
    input => '
      pushstr "hello world"
      call_builtin println(String) 1
      exit
      ',
    expected => '
      hello world
      pancake value_stack after execution finished:
      ----------------
      ----------------
      ',
  },
  {
    input => '
      label entry
      call foo() 0
      exit
      label foo()
      pushint 5
      pushint 3
      store foo
      call_builtin println(Sint) 1
      load foo
      call_builtin println(Sint) 1
      load foo
      call_builtin println(Sint) 1
      return_void
      ',
    expected => '
      5
      3
      3
      pancake value_stack after execution finished:
      ----------------
      ----------------
      ',
  },
  {
    input => '
      label entry
      call foo() 0
      exit
      label foo()
      pushbool 0
      pushbool 1
      call_builtin println(Bool) 1
      call_builtin println(Bool) 1
      pushuint 5
      pushuint 6
      call_builtin greaterthan_equal(Uint,Uint) 2
      call_builtin println(Bool) 1
      pushuint 5
      pushuint 6
      call_builtin greaterthan(Uint,Uint) 2
      call_builtin println(Bool) 1
      pushuint 5
      pushuint 6
      call_builtin lessthan_equal(Uint,Uint) 2
      call_builtin println(Bool) 1
      pushuint 5
      pushuint 6
      call_builtin lessthan(Uint,Uint) 2
      call_builtin println(Bool) 1
      pushint 5
      pushint 6
      call_builtin greaterthan_equal(Sint,Sint) 2
      call_builtin println(Bool) 1
      pushint 5
      pushint 6
      call_builtin greaterthan(Sint,Sint) 2
      call_builtin println(Bool) 1
      pushint 5
      pushint 6
      call_builtin lessthan_equal(Sint,Sint) 2
      call_builtin println(Bool) 1
      pushint 5
      pushint 6
      call_builtin lessthan(Sint,Sint) 2
      call_builtin println(Bool) 1
      return_void
      ',
    # 1
    # 0
    # 5 >= 6
    # 5 > 6
    # 5 <= 6
    # 5 < 6
    # 5 >= 6
    # 5 > 6
    # 5 <= 6
    # 5 < 6
    expected => '
      True
      False
      False
      False
      True
      True
      False
      False
      True
      True
      pancake value_stack after execution finished:
      ----------------
      ----------------
      ',
  },
];

# whitespace sensitivity sucks
sub cleanup {
  my $str = shift // die;
  die if @_;

  # strip any leading whitespace on each line
  $str =~ s/^\s*//mg;

  # strip any trailing whitespace on each line
  $str =~ s/\s*$//mg;

  # strip any blank lines
  $str =~ s/^\s*$//mg;

  return $str;
}

sub run {
  my $input = shift // die;
  my $expected = shift // die;
  die if @_;

  my $pid = open3(my $write, my $read, my $error, "$path -d --bytecode -") or die "Failed to open $path";
  print $write $input;
  close $write;
  waitpid($pid, 0);
  my $status = $?;
  my $output = "";
  {
      local $/ = undef;
      $output = <$read>;
  }
  $output = cleanup $output;

  if( $status != 0 ){
      say "Command return non-zero exit code '${status}";
      say "\n=====\nFor input:";
      say "$input";
      say "\n=====\nExpected output:";
      say "$expected";
      say "\n=====\nGot output:";
      say "$output";
      say "\n=====\n";
      die "Command return non-zero exit code '${status}";
  }

  if( $output ne $expected ){
      say "Output was not as expected";
      say "=======\nExpected:\n$expected";
      say "=======\nGot:\n$output";
      say "=======\n";
      die "Output not as expected";
  }

  say "=======\nGot correct output:\n$output\n=======";
}

for my $case (@$cases) {
  my $input = cleanup $case->{input};
  my $expected = cleanup $case->{expected};
  run $input, $expected;
}

say "test_backend_pancake_interpret_simple successs";

