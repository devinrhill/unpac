#include "arcv.h"
#include <cmath>
#include <filesystem>
#include <format>
#include <fstream>
#include "giga/lzss.hpp"
#include <iostream>
#include <raylib.h>
// #include "brres.h"
#include "g03_item_set.h"

constexpr float VERSION = 1.7;

#define NDEBUG 1

#if !NDEBUG
#include <rcamera.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Simple3DCamera {
public:
  void update() {
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
      cameraSpeed = defaultCameraSpeed * 8;
    } else {
      cameraSpeed = defaultCameraSpeed;
    }

    if (IsKeyDown(KEY_W)) {
      CameraMoveForward(&camera, cameraSpeed * GetFrameTime(), true);
    }
    if (IsKeyDown(KEY_A)) {
      CameraMoveRight(&camera, -cameraSpeed * GetFrameTime(), true);
    }
    if (IsKeyDown(KEY_S)) {
      CameraMoveForward(&camera, -cameraSpeed * GetFrameTime(), true);
    }
    if (IsKeyDown(KEY_D)) {
      CameraMoveRight(&camera, cameraSpeed * GetFrameTime(), true);
    }
    if (IsKeyDown(KEY_Q)) {
      CameraMoveUp(&camera, -cameraSpeed * GetFrameTime());
    }
    if (IsKeyDown(KEY_E)) {
      CameraMoveUp(&camera, cameraSpeed * GetFrameTime());
    }

    if (IsKeyDown(KEY_LEFT)) {
      camera.target = cameraFocus;
      CameraMoveRight(&camera, -cameraSpeed * GetFrameTime(), true);
    }
    if (IsKeyDown(KEY_RIGHT)) {
      camera.target = cameraFocus;
      CameraMoveRight(&camera, cameraSpeed * GetFrameTime(), true);
    }

    // UpdateCamera(&camera, CAMERA_FIRST_PERSON);
  }

  Camera &getCamera() { return camera; }

private:
  Vector3 cameraFocus = {0, 0, 0};
  const float defaultCameraSpeed = 16;
  float cameraSpeed = defaultCameraSpeed;
  Camera camera = {{2, 2, 2}, cameraFocus, {0, 1, 0}, 60, CAMERA_PERSPECTIVE};
};
#endif

int main(int argc, char **argv) {
  bool quiet = false;

  if (argc < 3) {
  usage:
    std::cerr << "Unpac by lvlrk, v" << VERSION << std::endl;
    std::cerr << "Usage: unpac [-q] <x|c|d|C|L|m> <filename>" << std::endl;
#if !NDEBUG
    std::cerr << "DEBUG MODE" << std::endl;
#endif
    return 1;
  }

  std::string program = argv[1];
  int aidx = 0;

  if(program == "-q" || program == "--quiet") {
    quiet = true;
    aidx++;
    program = argv[2];
  }

  if (argc >= 3) {
    std::string input, output;

    unpac::Arcv arcv;
    if (program == "x") {
      input = argv[2+aidx];
      output = std::filesystem::path(input).parent_path() /
               std::filesystem::path(input).stem();

      if(!quiet) {
        std::cout << std::format("extract: {} -> {}\n", input, output);
      }

      arcv.openFile(input);
      arcv.extractAll(output);

      return 0;
    } else if (program == "c") {
      input = argv[2+aidx];
      output = std::filesystem::path(argv[2+aidx]).parent_path() /
               std::filesystem::path(std::string(argv[2+aidx]) + ".lzs");

      if(!quiet) {
        std::cout << std::format("create: {} -> {}\n", input, output);
      }

      arcv.injectDirectory(input);
      arcv.finalizeFile(output, true);

      /*
      for(const giga::Bytestream& bytestream: arcv) {
          std::cout << bytestream.getFilename() << std::endl;
      }
      */

      return 0;
    } else if (program == "d") {
      input = argv[2+aidx];
      output = std::filesystem::path(input).stem().string() + ".arc";

      if(!quiet) {
        std::cout << std::format("decompress: {} -> {}\n", input, output);
      }

      arcv.openFile(input);
      arcv.finalizeFile(output, false);

      return 0;
    } else if (program == "C") {
      input = argv[2+aidx];
      output = std::filesystem::path(input).stem().string() + ".lzs";

      if(!quiet) {
        std::cout << std::format("compress: {} -> {}\n", input, output);
      }

      giga::Bytestream inBytestream;
      inBytestream.openFile(input);

      std::vector<std::uint8_t> uncompBuf(inBytestream.getSize());
      inBytestream.read(uncompBuf.data(), uncompBuf.size());

      std::vector<std::uint8_t> compBuf(uncompBuf.size());

      std::size_t newCompBufSize = giga::lzss::compress(
          uncompBuf.data(), uncompBuf.size(), compBuf.data(), compBuf.size(),
          giga::lzss::Config{12, 4});
      compBuf.resize(newCompBufSize);

      giga::Bytestream outBytestream;
      outBytestream.write(reinterpret_cast<const std::uint8_t *>("SSZL"), 4);
      outBytestream.writeScalar<std::uint32_t>(0);
      outBytestream.writeScalar<std::uint32_t>(newCompBufSize);
      outBytestream.writeScalar<std::uint32_t>(uncompBuf.size());
      outBytestream.write(compBuf.data(), newCompBufSize);

      outBytestream.finalizeFile(output);

      return 0;
    } else if (program == "L") {
      input = argv[2+aidx];

      if(!quiet) {
        std::cout << std::format("list: {}\n", input);
      }

      arcv.openFile(input);
      for (const giga::Bytestream &bytestream : arcv) {
        std::cout << bytestream.getFilename() << std::endl;
      }

      return 0;
    } else if (program == "m") {
      input = argv[2+aidx];
      output = std::string(argv[2+aidx]) + ".arc";

      if(!quiet) {
        std::cout << std::format("create: {} -> {}\n", input, output);
      }

      arcv.injectDirectory(input);
      arcv.finalizeFile(output, false);

      return 0;
#if NDEBUG
    }
#elif !NDEBUG
    } else if (program == "test") {
      std::string input = argv[2+aidx];

      unpac::Arcv arcv;
      arcv.openFile(input);

      input = std::filesystem::path(input).filename();

      json list;

      std::ifstream inListFile(
          "/home/devinrhill/Code/proj/workstation/unpac/list.json");
      if (std::filesystem::exists(
              "/home/devinrhill/Code/proj/workstation/unpac/list.json")) {
        list = json::parse(inListFile);
      }

      list[input] = {};
      for (const giga::Bytestream &bytestream : arcv) {
        list[input].push_back(bytestream.getFilename());
      }

      std::ofstream outListFile;
      outListFile.open(
          "/home/devinrhill/Code/proj/workstation/unpac/list.json");
      outListFile << list.dump(4);

      return 0;
    } else if (program == "P") {
      input = argv[2+aidx];

      arcv.openFile(input);

      unpac::g03::ItemSet itemSet;
      giga::Bytestream bytestream = arcv.getMember("BIN_ITEM_SET_DAT");
      itemSet.openBytestream(bytestream);

      for (unpac::g03::Item &item : itemSet) {
        std::cout << std::format("{}: {:.2f}, {:.2f}, {:.2f}\n",
                                 item.getTypeName(), item.x, item.y, item.z);
      }

      Simple3DCamera camera;

      float positionFactor = (1.0 / 12.0);
      Vector3 scale = {positionFactor * 0.27f, positionFactor * 0.5f,
                       positionFactor * 0.5f};

      InitWindow(1920, 1080, "unpac: play mode");
      while (!WindowShouldClose()) {
        camera.update();

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera.getCamera());
        // DrawCubeV({0, 0, 0}, {1, 1, 1}, GOLD);

        for (unpac::g03::Item &item : itemSet) {
          // DrawCubeV({item.x * positionFactor, item.y * positionFactor, item.z
          // * positionFactor}, scale, GOLD);
          DrawSphereEx({item.x * positionFactor, item.y * positionFactor,
                        item.z * positionFactor},
                       scale.x, 6, 10, GOLD);
        }
        EndMode3D();
        EndDrawing();
      }
      CloseWindow();

      return 0;
    } else if (program == "b") {
      /*
      input = argv[2+aidx];

      giga::Bytestream member;
      member.openFile(input);

      member.seek(0x80);

      unpac::Brres::Tex0 tex0;
      tex0.openBytestream(member);

      float scale = (1024.0f / (float)tex0.getWidth());

      if(tex0.getFormat() == unpac::Brres::Tex0::Format::RGBA32) {
          if(argc >= 4) {
              if(argv[3+aidx][0] == 'p') {
                  InitWindow(1024, 1024, "brres test");

                  Image image;
                  image.data = tex0.getPixelBuf();
                  image.width = tex0.getWidth();
                  image.height = tex0.getHeight();
                  image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
                  image.mipmaps = 1;

                  Texture2D texture = LoadTextureFromImage(image);

                  while(!WindowShouldClose()) {
                      BeginDrawing();
                          ClearBackground(BLACK);

                          DrawTextureEx(texture, {0, 0}, 0, scale, WHITE);
                      EndDrawing();
                  }
                  CloseWindow();
              }
          }
      } else {
          std::cerr << "Error: This file is not in RGBA32 format" << std::endl;

          return 1;
      }

      return 0;
      */
    } else if (program == "T") {
      /*
      input = argv[2+aidx];

      giga::Bytestream file;
      file.openFile(input);
      file.seek(0x80);

      unpac::Brres::Tex0 tex0;
      tex0.openBytestream(file);

      std::cout << tex0.getFormatName() << std::endl;

      return 0;
      */
    }
#endif
  }

  // last resort lol
  goto usage;

  return 0;
}
