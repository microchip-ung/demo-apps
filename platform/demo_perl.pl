


#my $filename = '../../../vtss_api/include/vtss/api/options.h';
my $filename = 'options.h';

open(my $fh, $filename) or die "Failed to open file $filename";

while (my $row = <$fh>) {
    chomp $row;

    if ($row =~ /^ *#define *([^ ]+)/) {
        $data{$1} = "1";
    }
}

print "#include <stdio.h>\n";
print "#include <vtss/api/options.h>\n";
print "#include \"demo_api.h\"\n";


print "void demo_options(void) {\n";

$x = VTSS_OPT_FDMA_VER;
print "#ifdef $x\n";
print "   printf(\" $x = %d\\n\", $x);\n";
print "#endif\n";
    
foreach $x (sort keys %data) {
        print "#ifdef $x\n";
        print "   printf(\" $x \\n\");\n";
        print "#endif\n"
}

foreach $x (sort keys %data) {
        print "#ifndef $x\n";
        print "   printf(\"! $x \\n\");\n";
        print "#endif\n"
}
print "}\n";
