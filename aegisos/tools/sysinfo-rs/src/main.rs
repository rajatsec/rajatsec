// AegisOS Phase 0 — M0.4: Rust systems tool
// Reads /proc to report kernel version, memory stats, running namespaces,
// and loaded security modules. Will evolve into the mode-status CLI in Phase 2.

use std::fs;
use std::io::{self, BufRead};
use std::path::Path;

fn main() {
    println!("=== AegisOS sysinfo ===\n");
    print_kernel_version();
    print_memory_info();
    print_loaded_lsms();
    print_namespace_info();
    print_cgroup_version();
}

fn print_kernel_version() {
    match fs::read_to_string("/proc/version") {
        Ok(v) => println!("[kernel]\n{}\n", v.trim()),
        Err(e) => eprintln!("[kernel] error reading /proc/version: {e}"),
    }
}

fn print_memory_info() {
    println!("[memory]");
    if let Ok(file) = fs::File::open("/proc/meminfo") {
        let reader = io::BufReader::new(file);
        for line in reader.lines().map_while(Result::ok) {
            // Print only the summary lines — not every entry
            if line.starts_with("MemTotal:")
                || line.starts_with("MemAvailable:")
                || line.starts_with("SwapTotal:")
                || line.starts_with("SwapFree:")
            {
                println!("  {line}");
            }
        }
    }
    println!();
}

fn print_loaded_lsms() {
    println!("[security modules]");
    // /sys/kernel/security/lsm lists the active LSM stack (kernel 4.17+)
    match fs::read_to_string("/sys/kernel/security/lsm") {
        Ok(lsms) => println!("  active LSMs: {}", lsms.trim()),
        Err(_) => println!("  /sys/kernel/security/lsm not readable (run as root or check kernel config)"),
    }
    println!();
}

fn print_namespace_info() {
    println!("[namespaces for this process]");
    let ns_path = Path::new("/proc/self/ns");
    match fs::read_dir(ns_path) {
        Ok(entries) => {
            let mut names: Vec<String> = entries
                .filter_map(Result::ok)
                .map(|e| e.file_name().to_string_lossy().to_string())
                .collect();
            names.sort();
            for ns in names {
                // Read the symlink target to show the inode (namespace ID)
                let link = fs::read_link(format!("/proc/self/ns/{ns}"))
                    .map(|p| p.to_string_lossy().to_string())
                    .unwrap_or_else(|_| "?".to_string());
                println!("  {ns:<20} -> {link}");
            }
        }
        Err(e) => eprintln!("  error reading /proc/self/ns: {e}"),
    }
    println!();
}

fn print_cgroup_version() {
    println!("[cgroup version]");
    // If /sys/fs/cgroup/cgroup.controllers exists, we're on cgroups v2
    if Path::new("/sys/fs/cgroup/cgroup.controllers").exists() {
        println!("  cgroups v2 (unified hierarchy)");
        match fs::read_to_string("/sys/fs/cgroup/cgroup.controllers") {
            Ok(c) => println!("  controllers: {}", c.trim()),
            Err(_) => {}
        }
    } else if Path::new("/sys/fs/cgroup").exists() {
        println!("  cgroups v1 (legacy hierarchy)");
    } else {
        println!("  cgroups not detected");
    }
    println!();
}
