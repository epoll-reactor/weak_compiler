#include "FrontEnd/Lex/Lexer.hpp"
#include "FrontEnd/Parse/Parser.hpp"
#include "MiddleEnd/CodeGen/CodeGen.hpp"
#include "MiddleEnd/Symbols/Storage.hpp"
#include "TestHelpers.hpp"
#include <fstream>
#include <filesystem>

namespace fe = weak::frontEnd;
namespace me = weak::middleEnd;

void RunFromFile(std::string_view Path) {
  std::cerr << "Testing file " << Path << "...\n";
  std::ifstream File(Path.data());
  std::string Program(
    (std::istreambuf_iterator<char>(File)),
    (std::istreambuf_iterator<char>()));
  me::Storage Storage;
  fe::Lexer Lex(&Storage, &*Program.begin(), &*Program.end());
  auto Tokens = Lex.Analyze();
  fe::Parser Parser(&*Tokens.begin(), &*Tokens.end());
  auto AST = Parser.Parse();
  me::CodeGen CodeGen(AST.get());
  CodeGen.CreateCode();
}

int main() {
  auto Directory = std::filesystem::directory_iterator(
    std::filesystem::current_path());
  for (const auto &File : Directory) {
    if (File.path().extension() == ".wl") {
      RunFromFile(File.path().native());
    }
  }
}