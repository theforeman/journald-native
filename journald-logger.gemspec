# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'journald/logger/version'

Gem::Specification.new do |spec|
  spec.name          = 'journald-logger'
  spec.version       = Journald::Logger::VERSION
  spec.authors       = ['Anton Smirnov']
  spec.email         = ['sandfox@sandfox.im']
  spec.summary       = %q{systemd-journal native logger}
  # spec.description   = %q{TODO: Write a longer description. Optional.}
  spec.homepage      = 'https://github.com/sandfox-im/journald-logger'
  spec.license       = 'MIT'

  spec.files         = `git ls-files -z`.split("\x0")
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ['lib']

  spec.extensions << 'ext/journald_logger/extconf.rb'

  spec.add_development_dependency 'bundler', '~> 1.6'
  spec.add_development_dependency 'rake'
  spec.add_development_dependency 'rake-compiler'
end