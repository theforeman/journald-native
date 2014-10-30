require 'bundler/gem_tasks'
require 'rake/extensiontask'

spec = Gem::Specification.load('journald-native.gemspec')
Rake::ExtensionTask.new('journald_native', spec)

task :build => :compile do
  # just add prerequisite
end
