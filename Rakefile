require "bundler/gem_tasks"
require "rake/extensiontask"
require "rspec/core/rake_task"
require "rubocop/rake_task"

spec = Gem::Specification.load("journald-native.gemspec")
Rake::ExtensionTask.new("journald_native", spec)
RSpec::Core::RakeTask.new(:spec)
RuboCop::RakeTask.new

task :build => :compile do
  # just add prerequisite
end
