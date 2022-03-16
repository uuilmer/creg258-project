const path = require("path");
const pkg = require("./package.json");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const buildPath = "./build/";

module.exports = {
  entry: ["./src/entry.js"],
  output: {
    path: path.join(__dirname, buildPath),
    filename: "[name].[fullhash].js",
  },
  mode: "production",
  target: "web",
  devtool: "source-map",
  module: {
    rules: [
      {
        test: /\.js$/,
        use: "babel-loader",
        exclude: path.resolve(__dirname, "./node_modules/"),
      },
      {
        test: /\.(glb|gltf)$/,
        use: [
          {
            loader: "file-loader",
            options: {
              outputPath: "assets/models/",
            },
          },
        ],
      },
      {
        test: /\.(jpe?g|png|gif|svg|tga|babylon|mtl|pcb|pcd|prwm|obj|mat|mp3|ogg)$/i,
        use: "file-loader",
        exclude: path.resolve(__dirname, "./node_modules/"),
      },
    ],
  },
  plugins: [new HtmlWebpackPlugin({ title: "three-seed project" })],
};
