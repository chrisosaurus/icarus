#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

use File::Basename;

# consume the progress file
# which maps milestone to progress for each backend
# format is:
#   milestone-name 2c-progress pancake-progress
#
# 2c-progress can be any of:
#   skip - do not test
#   fail-compile - fails to compile in icarus
#   fail-c-compiler - icarus compiles, fails under c compiler
#   fail-runtime - icarus compiles, c compiler compiles, runtime doesn't exit cleanly
#   fail-output - icarus compiles, c compiler compiles, runtime exits clean, output doesn't match
#   pass - icarus compiles, c compiler compiles, runtime exits clean, output matches expected
#
# pancake-progress can be any of:
#   skip - do not test
#   fail-runtime - fails to compile or run
#   fail-output - output is not as expected
#   pass - runs cleanly and output is as expected

# for each *.ic we:
#   2c backend:
#     compile with icarus to c
#     compile the c with a c compiler
#     run the output binary
#     capture exit status
#     compare stdout with matching *.exp file
#   pancake backend:
#     run through pancake backend
#     capture exit status
#     compare stdout with atching *.exp file

# consume progress file
my %progress;
open( my $fh, '<', 't/milestones/progress' ) or
  die "failed to open progress file : $!";

for my $line (<$fh>){
  my @parts = split /\s+/, $line;
  if( @parts != 3 ){
    die "Invalid length for (@parts), expected 3 parts";
  }

  my $milestone = $parts[0];
  my $progress_2c = $parts[1];
  die "unknown option '$progress_2c'" unless in_list($progress_2c, qw/skip fail-compile fail-c-compiler fail-runtime fail-outpiut pass/);
  my $progress_pancake = $parts[2];
  die "unknown option '$progress_pancake" unless in_list($progress_pancake, qw/skip fail-runtime fail-output pass/);

  $progress{$milestone} = {
    '2c' => $progress_2c,
    'pancake' => $progress_pancake,
  };
}

close $fh;

# go through milestone files
my @milestones = sort <t/milestones/*.ic>;

# check if a given item is in a list
# returns 1 if it is
# returns 0 if it is not
sub in_list {
  my $item = shift // die;
  my @list = @_;
  die "in_list: didn't get any items for list" unless @list;

  # using `eq` for strings
  if( grep {$item eq $_} @list ){
    return 1;
  }

  return 0;
}

# run a given command
# die if that command fails
# otherwise return the output of that command
sub run_cmd_die {
  my $cmd = shift // die "no command given";
  die "too many args" if @_;

  my $out = `$cmd`;
  my $ret = $?;

  if ($ret) {
    die "Failed to run command '${cmd}'";
  }

  return $out;
}

# run a given command
# return a boolean on success
#  1 => ran cleanly
#  0 => failed
sub run_cmd_bool {
  my $cmd = shift // die "no command given";
  die "too many args" if @_;

  my $out = `$cmd`;
  my $ret = $?;

  if ($ret) {
    return 0; # fail
  }

  return 1; # success
}

# run a given command
# return output on success (string)
# returns undef on failure
sub run_cmd_undef {
  my $cmd = shift // die "no command given";
  die "too many args" if @_;

  my $out = `$cmd`;
  my $ret = $?;

  if ($ret) {
    return undef;
  }

  return $out;
}

sub slurp {
  my $fn = shift // die;
  die if @_;

  my $contents = "";

  open(my $fh, '<', "$fn") or die "Failed to open file '$fn': $!";
  {
    $/ = undef;
    $contents = <$fh>;
  }
  close($fh);

  return $contents;
}

# compare expected output in file
# to actual output
# returns (1, $exp) if they are equal
# returns (0, $exp) i they are different
sub compare {
  my $exp_fn = shift // die;
  my $got = shift // die;
  die if @_;

  my $exp = slurp("$exp_fn");

  if( $exp eq $got ){
    # success
    return (1, $exp);
  }

  return (0, $exp);
}

sub check_2c {
  my $basename = shift // die;
  my $exp = shift // die;
  my $expected_progress = shift // die;
  die if @_;

  my $cout = "$basename.c";

  # remove all files we want to use
  run_cmd_die("rm -rf $cout");
  run_cmd_die("rm -rf $basename");

  {
    my $cmd_0 = "./icarus 2c t/milestones/${basename}.ic -o ${cout}";
    my $prog_0 = run_cmd_bool($cmd_0);
    if( not $prog_0 ){
      if( 'fail-compile' eq "$expected_progress" ){
        run_cmd_die("rm -rf $cout");
        run_cmd_die("rm -rf $basename");
        return; # success
      }
      die "check_2c:
           when running command '$cmd_0'
           we received exit code '$prog_0'
           this constitues a 'fail-compile'
           but our expected progress was '$expected_progress'"; # failure
    }
  }

  {
    my $cmd_1 = "cc ${cout} -o ${basename}";
    my $prog_1 = run_cmd_bool($cmd_1);
    if( not $prog_1 ){
      if( 'fail-ccompiler' eq "$expected_progress" ){
        run_cmd_die("rm -rf $cout");
        run_cmd_die("rm -rf $basename");
        return; # success
      }
      die "check_2c:
           when running command '$cmd_1'
           we received exit code '$prog_1'
           this constitues a 'fail-ccompiler'
           but our expected progress was '$expected_progress'"; # failure
    }
  }

  {
    my $cmd_2 = "./${basename}";
    my $runtime_output = run_cmd_undef($cmd_2);
    if( not defined $runtime_output ){
      if( 'fail-runtime' eq "$expected_progress" ){
        run_cmd_die("rm -rf $cout");
        run_cmd_die("rm -rf $basename");
        return; # success
      }
      die "check_2c:
           when running command '$cmd_2'
           the program crashed unexpectantly
           this constitues a 'fail-runtime'
           but our expected progress was '$expected_progress'"; # failure
    }

    my ($res, $exp) = compare($exp, $runtime_output);
    unless( 1 == $res ){
      if( 'fail-output' eq "$expected_progress" ){
        run_cmd_die("rm -rf $cout");
        run_cmd_die("rm -rf $basename");
        return;
      }
      say "check_2c:
           when running command '$cmd_2'
           the program gave us output that didn't match out expected
           this constitues a 'fail-output'";
      say "got output:
          '$runtime_output'";
      say "expected output:
          '$exp'";
      die "unexpected fail-output";
    }
  }

  if( 'pass' ne "$expected_progress" ){
    die "check_2c:
         for test '$basename'
         all steps passed
         but expected progress was '$expected_progress'";
  }

  # cleanup
  run_cmd_die("rm -rf $cout");
  run_cmd_die("rm -rf $basename");
}

sub check_pancake {
  my $basename = shift // die;
  my $exp = shift // die;
  my $expected_progress = shift // die;
  die if @_;

  {
    my $cmd_0 = "./icarus pancake t/milestones/${basename}.ic";
    my $runtime_output = run_cmd_undef($cmd_0);
    if( not defined $runtime_output ){
      if( 'fail-runtime' eq "$expected_progress" ){
        return; # success
      }
      die "check_pancake:
           when running command '$cmd_0'
           the program crashed unexpectantly
           this constitues a 'fail-runtime'
           but our expected progress was '$expected_progress'"; # failure
    }

    # FIXME TODO cannot yet get good results here
    # due to debugging noise
    my ($res, $exp) = compare($exp, $runtime_output);
    unless( 1 == $res ){
      if( 'fail-output' eq "$expected_progress" ){
        return
      }
      say "check_pancake
           when running command '$cmd_0'
           the program gave us output that didn't match out expected
           our expected progress was '$expected_progress'
           this constitues a 'fail-output'";
      say "got output:
          '$runtime_output'";
      say "expected output:
          '$exp'";
      die "unexpected fail-output";
    }
  }

  if( 'pass' ne "$expected_progress" ){
    die "check_pancake:
         for test '$basename'
         all steps passed
         but expected progress was '$expected_progress'";
  }
}

for my $milestone (@milestones){
  my $basename = basename($milestone, qw/.ic/);
  my $exp = $milestone;
  $exp =~ s/.ic$/.exp/;
  if( ! -e "$exp" ){
    die "Could not find exp file '$exp' for ic file '$milestone' with basename '$basename'";
  }
  if( ! exists($progress{$basename}) ){
    die "Could not find expected progress for milestone '$milestone'";
  }
  my $progress_2c = $progress{$basename}->{"2c"} || die;
  my $progress_pancake = $progress{$basename}->{"pancake"} || die;

  if( 'skip' ne "$progress_2c" ){
    check_2c($basename, $exp, $progress_2c);
  }

  if( 'skip' ne "$progress_pancake" ){
    check_pancake($basename, $exp, $progress_pancake);
  }
}

say "all milestones match t/milestones/progress";

# FIXME TODO would be better to perform all work in temporary dir

