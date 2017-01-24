proc MkIndex {} {
    puts "making tclIndex for current directory [pwd]"
    auto_mkindex . *.tcl
    catch {exec chmod 440 tclIndex}
}
