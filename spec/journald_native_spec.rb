require_relative 'spec_helper'

# I don't know how to mock inside the C extension so just call real journal methods

RSpec.describe Journald::Native do
  describe 'sends log entries' do
    it 'calls sd_journal_send()' do
      # long function name
      expect(
        Journald::Native.sd_journal_send(
          "PRIORITY=#{Journald::LOG_DEBUG}",
          'MESSAGE=test send()',
          'SOMEFIELD=some value'
        )
      ).to eq(0)

      # short function name
      expect(
        Journald::Native.send(
          "PRIORITY=#{Journald::LOG_DEBUG}",
          'MESSAGE=test send()',
          'SOMEFIELD=some value'
        )
      ).to eq(0)
    end

    it 'calls sd_journal_print()' do
      # long function name
      expect(
        Journald::Native.sd_journal_print(Journald::LOG_DEBUG, 'test print()')
      ).to eq(0)

      # short function name
      expect(
        Journald::Native.print(Journald::LOG_DEBUG, 'test print()')
      ).to eq(0)
    end

    it 'calls sd_journal_perror()' do
      # long function name
      expect(Journald::Native.sd_journal_perror('test perror()')).to eq(0)
      # short function name
      expect(Journald::Native.perror('test perror()')).to eq(0)
    end

    it 'calls send() with \0 symbol' do
      expect(
        Journald::Native.sd_journal_send(
          "PRIORITY=#{Journald::LOG_DEBUG}",
          "MESSAGE=test\0sd_journal_send() with zero",
          "SOMEFIELD=some value\0with zero"
        )
      ).to eq(0)
    end

    it 'fails on print() with \0 symbol' do
      expect do
        Journald::Native.sd_journal_print(
          Journald::LOG_DEBUG,
          "test sd_journal_print()\0with zero"
        )
      end.to raise_error(ArgumentError, 'string contains null byte')
    end

    it 'fails on perror() with \0 symbol' do
      expect do
        Journald::Native.sd_journal_perror "test perror()\0with zero"
      end.to raise_error(ArgumentError, 'string contains null byte')
    end
  end
end
