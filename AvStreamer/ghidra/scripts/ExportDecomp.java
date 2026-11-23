// Export decompiled C for every function in the current program, plus a
// function index (address, size, name, source file) and the defined strings.
// Usage (headless): -postScript ExportDecomp.java <outdir>
//@category Export
import ghidra.app.decompiler.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.data.StringDataInstance;
import java.io.*;

public class ExportDecomp extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        File outDir = new File(args.length > 0 ? args[0] : ".");
        outDir.mkdirs();
        String base = currentProgram.getName();

        DecompInterface di = new DecompInterface();
        DecompileOptions opts = new DecompileOptions();
        di.setOptions(opts);
        di.toggleCCode(true);
        di.toggleSyntaxTree(false);
        di.setSimplificationStyle("decompile");
        di.openProgram(currentProgram);

        try (PrintWriter c = new PrintWriter(new FileWriter(new File(outDir, base + "_decompiled.c")));
             PrintWriter idx = new PrintWriter(new FileWriter(new File(outDir, base + "_functions.tsv")))) {
            idx.println("address\tsize\tthunk/external\tname\tsignature");
            FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
            int n = 0;
            while (it.hasNext() && !monitor.isCancelled()) {
                Function f = it.next();
                boolean ext = f.isExternal() || f.isThunk();
                idx.printf("%s\t%d\t%s\t%s\t%s%n", f.getEntryPoint(), f.getBody().getNumAddresses(),
                        ext ? "Y" : "", f.getName(true), f.getSignature().getPrototypeString());
                if (ext) continue;
                DecompileResults r = di.decompileFunction(f, 120, monitor);
                c.printf("%n// ==== %s @ %s ====%n", f.getName(true), f.getEntryPoint());
                if (r != null && r.decompileCompleted()) {
                    c.println(r.getDecompiledFunction().getC());
                } else {
                    c.println("// decompile failed: " + (r == null ? "null" : r.getErrorMessage()));
                }
                n++;
            }
            println("Decompiled " + n + " functions");
        }

        try (PrintWriter s = new PrintWriter(new FileWriter(new File(outDir, base + "_strings.tsv")))) {
            DataIterator di2 = currentProgram.getListing().getDefinedData(true);
            while (di2.hasNext()) {
                Data d = di2.next();
                if (d.hasStringValue()) {
                    Object v = d.getValue();
                    s.printf("%s\t%s%n", d.getAddress(), String.valueOf(v).replace("\n", "\\n").replace("\t", "\\t"));
                }
            }
        }
        di.dispose();
    }
}
