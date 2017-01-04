#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

# a 'proof' of one.ic being roughly correct

# 0 based heap
# so parent is (i-1)/2
# left child is 2i + 1
# right child is 2i + 2

sub lchild {
    my $i = shift // die;
    return (2 * $i) + 1;
}

sub rchild {
    my $i = shift // die;
    return (2 * $i) + 2;
}

sub parent {
    my $i = shift // die;
    return int(($i - 1) / 2);
}

sub heapify {
    my $a = shift // die;
    my $len = @$a;

    for my $i (1..($len-1)){
        pushup($a, $i);
    }
}

sub pushup {
    my $a = shift // die;
    my $i = shift // die;

    if( $i <= 0 ){
        return;
    }

    my $v = $a->[$i];
    my $pv = $a->[(parent($i))];

    if( $v > $pv ){
        swap($a, $i, parent($i));
        pushup($a, parent($i));
    }
}

sub swap {
    my $a = shift // die;
    my $l = shift // die;
    my $r = shift // die;

    my $t = $a->[$l];
    $a->[$l] = $a->[$r];
    $a->[$r] = $t;
}

sub max {
    my $a = shift // die;
    my $b = shift // die;
    my $c = shift // die;

    if( $a >= $b ){
        if( $a >= $c ){
            return $a;
        }
    } else {
        if( $b >= $c ){
            return $b;
        }
    }

    return $c;
}

sub heapmax {
    my $a = shift // die;
    my $i = shift // die;
    my $u = shift // die;

    my $li = lchild($i);
    my $ri = rchild($i);

    if( $li > $u ){
        return $i;
    }

    if( $ri > $u ){
        if( $a->[$i] >= $a->[$li] ){
            return $i;
        } else {
            return $li;
        }
    }

    if( $a->[$i] >= $a->[$li] ){
        if( $a->[$i] >= $a->[$ri] ){
            return $i;
        }
    } else {
        if( $a->[$li] >= $a->[$ri] ){
            return $li;
        }
    }

    return $ri;
}

sub pushdown {
    my $a = shift // die;
    my $i = shift // die;
    my $u = shift // die;

    if( $i >= $u ){
        return;
    }

    my $si = heapmax($a, $i, $u);
    if( $si == $i ){
        return;
    }

    swap($a, $i, $si);
    pushdown($a, $si, $u);
}

sub sortheap {
    my $a = shift // die;
    my $len = @$a;

    for my $i (reverse(0..($len-1))){
        swap($a, 0, $i);
        pushdown($a, 0, $i-1);
    }
}

sub heapsort {
    my $a = shift // die;
    heapify($a);
    sortheap($a);
}

sub print_array {
    my $a = shift // die;
    say join ",", @$a;
}

sub make_array {
    my $a = [];

    for (1..7){
        push @$a, int(rand(100));
    }

    return $a;
}

sub main {
    my $a = make_array();
    heapsort($a);
    print_array($a);
}

main();

