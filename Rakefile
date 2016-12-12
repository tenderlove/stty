# -*- ruby -*-

require 'rubygems'

def auto_install file, gem = file
  retried = false
  require file
rescue LoadError
  raise if retried
  Gem.install gem
  retried = true
  retry
end

auto_install 'hoe'
auto_install 'rake/extensiontask', 'rake-compiler'

Hoe.plugin :minitest
Hoe.plugin :gemspec # `gem install hoe-gemspec`
Hoe.plugin :git     # `gem install hoe-git`

Hoe.spec 'stty' do
  developer('Aaron Patterson', 'tenderlove@ruby-lang.org')
  self.readme_file   = 'README.md'
  self.history_file  = 'CHANGELOG.md'
  self.extra_rdoc_files  = FileList['*.md']
  license 'MIT'

  self.extra_dev_deps += [
    ["rake-compiler"],
    ["minitest", '~> 5.0'],
  ]
end

Rake::ExtensionTask.new("stty") do |ext|
  ext.lib_dir = File.join(*['lib', ENV['FAT_DIR']].compact)
end

# vim: syntax=ruby
