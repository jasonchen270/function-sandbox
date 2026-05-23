// function-sandbox <function>
//
// Try a change to one function WITHOUT touching your source. It makes a
// throwaway copy of the project where every file is a SYMLINK back to your
// original, except the one file that defines the function, which is a real
// (writable) copy. That's the only file you edit. It does NOT edit, build, or
// run anything; you do that in the copy, then delete it. Run from a project root.
//
//   build:  ./build.sh        (-> ./function-sandbox)
//   use:    cd <your project> && /path/to/function-sandbox <function>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

bool is_source(const fs::path& p) {
    static const char* ext[] = {".cpp", ".cc", ".cxx", ".c", ".h", ".hpp", ".hh"};
    auto e = p.extension().string();
    for (auto x : ext)
        if (e == x) return true;
    return false;
}

bool excluded(const fs::path& rel) {
    for (const auto& part : rel) {
        auto s = part.string();
        if (s == "build" || s == ".git" || s == ".DS_Store") return true;
        if (s.find(".sandbox-") != std::string::npos) return true;  // our own copies
    }
    return rel.extension() == ".bak";
}

std::string read_file(const fs::path& p) {
    std::ifstream in(p, std::ios::binary);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

// First file:line where `name` is DEFINED (a body), searching the whole tree.
// A definition is the only form where the next '{' precedes the next ';'.
bool locate(const fs::path& root, const std::string& name,
            std::string& rel_out, int& line_out) {
    std::vector<fs::path> files;
    for (auto it = fs::recursive_directory_iterator(root);
         it != fs::recursive_directory_iterator(); ++it) {
        if (excluded(fs::relative(it->path(), root))) {
            if (it->is_directory()) it.disable_recursion_pending();
            continue;
        }
        if (it->is_regular_file() && is_source(it->path()))
            files.push_back(it->path());
    }
    std::sort(files.begin(), files.end());

    std::regex re("\\b" + name + "\\b\\s*\\(");
    for (const auto& path : files) {
        std::string text = read_file(path);
        for (auto m = std::sregex_iterator(text.begin(), text.end(), re);
             m != std::sregex_iterator(); ++m) {
            std::size_t after = m->position() + m->length();
            std::size_t semi = text.find(';', after);
            std::size_t brace = text.find('{', after);
            if (brace == std::string::npos) continue;
            if (semi != std::string::npos && semi < brace) continue;  // proto/call
            line_out = 1 + (int)std::count(text.begin(),
                                           text.begin() + m->position(), '\n');
            rel_out = fs::relative(path, root).string();
            return true;
        }
    }
    return false;
}

// Recreate the tree: symlink every file back to the original, except `edited`,
// which is a real copy. Excluded dirs are pruned, not descended.
void build_sandbox(const fs::path& src, const fs::path& dst, const fs::path& edited) {
    fs::create_directories(dst);
    for (auto it = fs::recursive_directory_iterator(src);
         it != fs::recursive_directory_iterator(); ++it) {
        fs::path rel = fs::relative(it->path(), src);
        if (excluded(rel)) {
            if (it->is_directory()) it.disable_recursion_pending();
            continue;
        }
        fs::path target = dst / rel;
        if (it->is_directory())
            fs::create_directories(target);
        else if (rel == edited)
            fs::copy_file(it->path(), target, fs::copy_options::overwrite_existing);
        else
            fs::create_symlink(it->path(), target);  // absolute -> the original
    }
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: function-sandbox <function>\n";
        return 1;
    }
    std::string fn = argv[1];
    fs::path root = fs::current_path();

    std::string rel;
    int line = 0;
    if (!locate(root, fn, rel, line)) {
        std::cerr << "no definition of " << fn << "() found under " << root.string() << "\n";
        return 1;
    }

    fs::path dest = root;
    dest += ".sandbox-" + fn;  // sibling dir: <root>.sandbox-<fn>
    if (fs::exists(dest)) {
        std::cerr << dest.string() << " already exists; remove it first:  rm -rf '"
                  << dest.string() << "'\n";
        return 1;
    }
    build_sandbox(root, dest, rel);

    std::cout
        << "made a throwaway copy (symlinks to your originals; your source is untouched):\n  "
        << dest.string() << "\n\nedit " << fn << "() there (the one real file):\n  "
        << rel << ":" << line
        << "\n\nthen build & run it yourself, e.g.:\n  cd '" << dest.string()
        << "' && ./build.sh && ./build/app"
        << "\n\ndelete it when done:\n  rm -rf '" << dest.string() << "'\n";
    return 0;
}
