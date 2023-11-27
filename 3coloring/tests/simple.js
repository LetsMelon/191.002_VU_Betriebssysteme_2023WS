const util = require('node:util');
const exec = util.promisify(require('node:child_process').exec);

const main = async () => {
  let promises = [exec('./supervisor'), exec('./generator 0-1 0-2 1-2 0-3')];

  await Promise.all(promises);
}

main();
