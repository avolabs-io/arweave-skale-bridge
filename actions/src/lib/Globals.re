include Belt;
include Async;

let oMap = Belt.Option.map;
let (<$>) = oMap;
let oFlatMap = Belt.Option.flatMap;
let (>>=) = oFlatMap;
let mapd = Option.mapWithDefault;
let (||||) = Option.getWithDefault;
