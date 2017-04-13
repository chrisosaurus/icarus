#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

use File::Copy;

# This script is to automatically update the libs/* directories
# which are taken from various other git repositories

my $working_space = "update_libs_working_space";

my %libs = (
  linear_hash => {
    repo => 'git@github.com:mkfifo/linear_hash.git',
    files => [qw/
      linear_hash.c
      linear_hash.h
      linear_hash_internal.h
      test_linear_hash.c
    /],
  },

  linear_set => {
    repo => 'git@github.com:mkfifo/linear_set.git',
    files => [qw/
      linear_set.c
      linear_set.h
      linear_set_internal.h
      test_linear_set.c
    /],
  },
);

main();

sub main {
  my @changed = ();

  say "";

  abort_if_unsafe();

  if (-e "$working_space") {
    die "working_space '$working_space' already exists, exiting to prevent damage";
  }

  mkdir "$working_space";

  for my $lib (keys %libs) {
    say "updating library $lib";

    my $repo = $libs{$lib}->{repo};
    my $from_path = "${working_space}/${lib}";
    my $to_path = "libs/${lib}";

    if (-e "$from_path") {
      die "target checkout path already exists '$from_path'";
    }

    if (! -e "$to_path") {
      die "target dir doesn't exist'$to_path'";
    }

    # perform git clone
    my $git_clone = "git clone ${repo} ${from_path}";
    run_cmd($git_clone);

    # copy across each file
    for my $file (@{$libs{$lib}->{files}}){
      my $fpath = "${from_path}/${file}";
      my $tpath = "${to_path}/${file}";
      copy($fpath, $tpath) or die "Failed to copy '$fpath' to '$tpath': $!";
      run_cmd("git add $tpath")
    }

    # update version file
    my $log_cmd = "git --git-dir=${from_path}/.git log -1";
    my $version_data = run_cmd($log_cmd);
    my $vpath = "${to_path}/version";
    open (my $fh, '>', "$vpath") or die "Failed to open file '$vpath': $!";
    print $fh $version_data;
    close $fh;
    run_cmd("git add $vpath");

    my $changes = check_for_changes();
    if ($changes) {
      push @changed, $lib;
      say "Changes found, making a commit for '$lib'";
      # Make a commit of these changes
      run_cmd("git commit -m 'updating libs/${lib}'");
      say "Made a commit of these changes, please test";
    } else {
      say "No changes found, libs/${lib} was already up to date";
    }

    say "";
  }

  # clean up
  my $rm_cmd = "rm -rf ${working_space}";
  run_cmd($rm_cmd);

  if (@changed) {
    say "The following libs were updated and have had commits made:";
    for my $lib (@changed) {
      say "  $lib";
    }
    say "\nPlease make sure to test before pushing";
  } else {
    say "\nNo changes were made, all libs/ were up to date";
  }
  say "Finished";
}

# run a given command
# die if that command fails
# otherwise return the output of that command
sub run_cmd {
  my $cmd = shift // die "no command given";
  die "too many args" if @_;

  my $out = `$cmd`;
  my $ret = $?;

  if ($ret) {
    die "Failed to run command '${cmd}'";
  }

  return $out;
}

# check for local uncommitted git changes
# returns 1 if there are changes
# returns 0 if there are no changes
sub check_for_changes {
  my $cmd = "git status --porcelain | grep -v '^?? '";
  my $result = `$cmd`;
  my $ret = $?;

  if (0 == $ret) {
    # changes found
    return 1;
  } else {
    # no changes found
    return 0;
  }
}

# abort if there are local uncommitted git changes
sub abort_if_unsafe {
  die "too many args" if @_;

  my $changes = check_for_changes();

  if ($changes) {
    die "Found local uncommitted changes, aborting";
  }
}

