require_relative 'spec_helper'

# I don't know how to mock inside the C extension so just call real journal methods

RSpec.describe Journald::Native do
  describe 'sends log entries' do
    it 'calls send()' do
      expect(
          Journald::Native.send(
              "PRIORITY=#{Journald::LOG_DEBUG}",
              'MESSAGE=test send()',
              'SOMEFIELD=some value'
          )
      ).to eq(0)
    end

    it 'calls print()' do
      expect(Journald::Native.print Journald::LOG_DEBUG, 'test print()').to eq(0)
    end

    it 'calls perror()' do
      expect(Journald::Native.perror 'test perror()').to eq(0)
    end

    it 'calls send() with \0 symbol' do
      expect(
          Journald::Native.send(
              "PRIORITY=#{Journald::LOG_DEBUG}",
              "MESSAGE=test\0send() with zero",
              "SOMEFIELD=some value\0with zero"
          )
      ).to eq(0)
    end

    it 'fails on print() with \0 symbol' do
      expect{ Journald::Native.print Journald::LOG_DEBUG, "test print()\0with zero" }.to raise_error(ArgumentError, 'string contains null byte')
    end

    it 'fails on perror() with \0 symbol' do
      expect{ Journald::Native.perror "test perror()\0with zero" }.to raise_error(ArgumentError, 'string contains null byte')
    end
  end
end
