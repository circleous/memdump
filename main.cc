#include "memdump.h"

void print_usage(const char* kProg)
{
	fprintf(stderr, "Usage: %sp <pid> [-a/--all] [-d <outdir>] [-h/--help]\n", kProg);
	fprintf(stderr, "\t-h/--help: Print this help.\n");
	fprintf(stderr, "\t-a/--all: Dump all memory region.\n");
	fprintf(stderr, "\t-d <outdir>: Output directory for dump files.\n");
}

void dump_region(const uintptr_t start, const uintptr_t end, const std::string &outdir, const int pid, const bool ismem)
{
	const std::string prefix = ismem ? "/mem_" : "/map_";
	
	std::stringstream outpath;
	outpath << std::hex << outdir << prefix << start << "_" << end << ".bin";

	fprintf(stderr, "memdump: Output file %s\n", outpath.str().c_str());

	struct stat st = {0};
	if (stat(outdir.c_str(), &st) == -1)
		mkdir(outdir.c_str(), 0655);

	std::ofstream outfile(outpath.str());
	if (!outfile) {
		fprintf(stderr, "memdump: Error opening output file.\n");
		exit(3);
	}

	std::ifstream procmem("/proc/" + std::to_string(pid) + "/mem");
	if (!procmem) {
		fprintf(stderr, "memdump: Error opening /proc/%d/mem.\n", pid);
		exit(3);
	}

	procmem.seekg(start);

	std::istreambuf_iterator<char> infile_it(procmem);
	std::ostreambuf_iterator<char> outfile_it(outfile);

	std::copy_n(infile_it, end - start, outfile_it);

	outfile.close();
}

void dump_all(const std::vector<maps_lines> &maps, const std::string &outdir, const int pid)
{
	for (auto& region : maps) {
		dump_region(region.start, region.end, outdir, pid, region.path.empty());
	}
}

void parse_maps(const int pid, std::vector<maps_lines> &maps)
{
	std::ifstream proc_maps("/proc/" + std::to_string(pid) + "/maps");

	if (!proc_maps) {
		fprintf(stderr, "memdump: Error opening /proc/%d/maps\n", pid);
		exit(3);
	}

	std::string line;
	while (std::getline(proc_maps, line)) {
		maps_lines parse(line);
		maps.push_back(parse);
	}

	proc_maps.close();
}

int main(int argc, char* argv[])
{
	// TODO: Saner argument parser. eta : never
	if (argc < 2) {
		print_usage(argv[0]);
		return 1;
	}

	InputParser input(argc, argv);
	if (input.cmdOptionExists("-h") || input.cmdOptionExists("--help")) {
		print_usage(argv[0]);
		return 0;
	}

	const int kPid = std::strtol(argv[1], NULL, 10);
	bool all = false;

	std::string outdir = input.getCmdOption("-d");
	if (outdir.empty())
		outdir = '.';

	if (input.cmdOptionExists("--all") || input.cmdOptionExists("-a"))
		all = true;

	int status;
	ptrace(PTRACE_ATTACH, kPid, nullptr, 0);
	waitpid(kPid, &status, WSTOPPED);

	std::vector<maps_lines> maps;
	parse_maps(kPid, maps);

	if (!all) {
		for (size_t i = 0; i < maps.size(); ++i)
			fprintf(stdout, "[%zu] %s\n", i, maps[i].str().c_str());
		
		decltype(maps.size()) region;
		fprintf(stdout, "Select Region: ");
		// TODO: Handle when input select out of list. High priority.
		scanf("%zu", &region);
		// assert(region < maps.size());

		dump_region(maps[region].start, maps[region].end, outdir, kPid, maps[region].path.empty());
	} else {
		dump_all(maps, outdir, kPid);
	}

	ptrace(PTRACE_DETACH, kPid, nullptr, 0);
	fprintf(stderr, "memdump: Done!\n");

	return 0;
}
