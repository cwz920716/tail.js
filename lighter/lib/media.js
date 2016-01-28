// Generated by CoffeeScript 1.4.0
(function() {
  var ObjectId, fs, helper, media;

  require('should');

  helper = (require('../modules/helper'))();

  media = (require(__dirname + '/init')).media;

  fs = require('fs');

  ObjectId = require('mongodb').ObjectID;

  describe('Media', function() {
    var buffer;
    buffer = null;
    before(function(done) {
      return fs.readFile(__dirname + '/../public/logo.png', function(err, result) {
        var promise;
        buffer = result;
        if (Buffer.isBuffer(buffer)) {
          promise = media.create({
            id: new ObjectId(),
            slug: 'logo.png',
            type: 'image/png',
            body: buffer
          });
          return promise.then(function(result) {
            return done();
          });
        }
      });
    });
    it('should assert the content created', function(done) {
      var promise, url;
      url = helper.linkify('logo.png');
      promise = media.get(url);
      return promise.then(function(result) {
        var i, _i, _ref;
        Buffer.isBuffer(result.data).should.be.ok;
        result.data.length.should.equal(buffer.length);
        result.type.should.equal('image/png');
        for (i = _i = 0, _ref = result.length; 0 <= _ref ? _i < _ref : _i > _ref; i = 0 <= _ref ? ++_i : --_i) {
          result[i].data.should.equal(buffer[i]);
        }
        return done();
      });
    });
    return after(function(done) {
      var url;
      url = helper.linkify('logo.png');
      return media.remove(url, function() {
        return done();
      });
    });
  });

}).call(this);
