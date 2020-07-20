include Belt;
include Async;
module Prometo = Yawaramin__Prometo;

module AsyncWrap = {
  let let_ = mapAsync;
};

let oMap = Belt.Option.map;
let (<$>) = oMap;
let oFlatMap = Belt.Option.flatMap;
let (>>=) = oFlatMap;
let mapd = Option.mapWithDefault;
let (||||) = Option.getWithDefault;
